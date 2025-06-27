#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int rank, ntasks, hostname_len;
  char hostname[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(hostname, &hostname_len);

  printf("Hello from %d of %d from %s!\n", rank, ntasks, hostname);
  fflush(stdout);
  if (rank == 0)
  {
    printf("Total of %d MPI processes\n", ntasks);
    fflush(stdout);
  }
  else if (rank == (ntasks-1))
  {
    printf("I'm last, but not least\n");
    fflush(stdout);
  }
  else if (rank == 42)
  {
    printf("I'm the Answer to the Ultimate Question of Life, the Universe, and Everything!\n");
    fflush(stdout);
  }

  MPI_Finalize();
}
