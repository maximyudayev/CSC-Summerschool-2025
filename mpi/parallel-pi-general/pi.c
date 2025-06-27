#include <stdio.h>
#include <math.h>
#include <mpi.h>


double calc_pi_partial(int n, int istart, int istop) {
  double pi = 0.0;
  for (int i=istart; i <= istop; i++) {
    double x = (i - 0.5) / n;
    pi += 1.0 / (1.0 + x*x);
  }
  return pi * 4.0 / n;
}


int main(int argc, char* argv[]) {
  int ntasks, rank, n = 840;
  double pi = 0.0, pi_ext;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  printf("Computing approximation to pi with N=%d\n", n);
  pi = calc_pi_partial(n, n/ntasks * rank + 1, n/ntasks * (rank + 1));

  int dst = rank > 0 ? 0 : MPI_PROC_NULL;

  MPI_Send(&pi, 1, MPI_DOUBLE, dst, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    for (int i = 1; i < ntasks; i++) {
      MPI_Recv(&pi_ext, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      pi += pi_ext;
    }
    printf("Approximate pi=%18.16f (exact pi=%10.8f)\n", pi, M_PI);
  }

  MPI_Finalize();
  return 0;
}
