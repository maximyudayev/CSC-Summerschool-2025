#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>


int randomMessageLength();


int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, ntasks;
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (ntasks < 2) {
    printf("Please run with at least 2 MPI processes\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // just some tag for the message
  const int tag = 123;

  if (rank == 1) {
    // Generate random message size in rank 1 only. Other ranks do not know the size
    int messageLength = randomMessageLength();
    int* message = (int*) malloc(messageLength * sizeof(int));

    // fill in a test message: element i has value i
    for (int i = 0; i < messageLength; i++) {
      message[i] = i;
    }

    // Send the test message to rank 0
    printf("Rank 1: Sending %d integers to rank 0\n", messageLength);
    fflush(stdout);
    MPI_Send(message, messageLength, MPI_INT, 0, tag, MPI_COMM_WORLD);
  }

  if (rank == 0) {
    MPI_Status status;
    int bufferLength;

    MPI_Probe(1, tag, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &bufferLength);
    printf("Rank 0: Received %d integers from rank 1.\n", bufferLength);
    fflush(stdout);
    int* buffer = (int*) malloc(bufferLength * sizeof(int));

    // Receive the message. Will error with MPI_ERR_TRUNCATE if the buffer is too small for the incoming message
    MPI_Recv(buffer, bufferLength, MPI_INT, 1, tag, MPI_COMM_WORLD, &status);
    // Print the received numbers
    for (int i = 0; i < bufferLength; i++ ) {
      printf("receiveBuffer[%d] : %d\n", i, buffer[i]);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}

// Helper functions below, no need to modify these

// Generate random int in specified range (inclusive), using Mersenne Twister
int randomInt(int min, int max) {
  return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

// Generates a random message length for the test message
int randomMessageLength() {
  int res = randomInt(2, 10);
  assert(res > 0 && "Can't happen: generated nonsensical message length...");
  return res;
}
