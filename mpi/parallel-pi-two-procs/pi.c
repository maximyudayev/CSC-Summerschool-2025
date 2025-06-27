#include <stdio.h>
#include <math.h>
#include <mpi.h>


double calc_pi_partial(int n, int istart, int istop) {
  double pi = 0.0;
  for (int i=istart+1; i <= istop; i++) {
    double x = (i - 0.5) / n;
    pi += 1.0 / (1.0 + x*x);
  }
  return pi * 4.0 / n;
}


int main(int argc, char* argv[]) {
  int ntasks, rank, n = 10000;
  double pi = 0.0, pi_ext;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (ntasks > 2) {
    printf("Please run with at least 2 MPI processes\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  if (rank == 0) {
    printf("Computing approximation to pi with N=%d\n", n);
    pi = calc_pi_partial(n, 0, n/2);
    MPI_Recv(&pi_ext, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    pi += pi_ext;
  }
  else if (rank == 1) {
    pi = calc_pi_partial(n, n/2, n);
    MPI_Send(&pi, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    printf("Approximate pi=%18.16f (exact pi=%10.8f)\n", pi, M_PI);
  }

  MPI_Finalize();
  return 0;
}
