/* Main solver routines for heat equation solver */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include <omp.h>
#include "heat.h"


void exchange_immediate(field* temperature, parallel_data* parallel, MPI_Request* requests)
{
  double *sbuf_up, *sbuf_down, *rbuf_up, *rbuf_down;

  // halo exchange
  // NOTE: ny and nx are flipped.
  sbuf_up = temperature->data + (temperature->ny+2);
  sbuf_down = temperature->data + (temperature->nx)*(temperature->ny+2);
  rbuf_up = temperature->data;
  rbuf_down = temperature->data + (temperature->nx+1)*(temperature->ny+2);

  // Send to up, receive from down
  MPI_Isend(sbuf_up, temperature->ny+2, MPI_DOUBLE, parallel->nup, 0, MPI_COMM_WORLD, requests);
  MPI_Irecv(rbuf_down, temperature->ny+2, MPI_DOUBLE, parallel->ndown, 0, MPI_COMM_WORLD, requests+1);

  // Send to down, receive from up
  MPI_Isend(sbuf_down, temperature->ny+2, MPI_DOUBLE, parallel->ndown, 0, MPI_COMM_WORLD, requests+2);
  MPI_Irecv(rbuf_up, temperature->ny+2, MPI_DOUBLE, parallel->nup, 0, MPI_COMM_WORLD, requests+3);
}


void exchange_wait(field* temperature, parallel_data* parallel, MPI_Request* requests)
{
  MPI_Status statuses[4];
  // Wait for all requests to finish
  MPI_Waitall(4, requests, statuses);
}


void five_point_stencil(int* i, int* j, int* ny,
                        double* a, double* dt, double* dx2, double* dy2,
                        double* prevdata, double* currdata)
{
  int ind = *i * (*ny + 2) + *j;
  int ip = (*i + 1) * (*ny + 2) + *j;
  int im = (*i - 1) * (*ny + 2) + *j;
  int jp = *i * (*ny + 2) + *j + 1;
  int jm = *i * (*ny + 2) + *j - 1;
  currdata[ind] = prevdata[ind] + *a * *dt *
       	          ((prevdata[ip] - 2.0 * prevdata[ind] + prevdata[im]) / *dx2 +
	          (prevdata[jp] - 2.0 * prevdata[ind] + prevdata[jm]) / *dy2);
}


// calculate field in the local subgrid
void evolve_local(field *curr, field *prev, double a, double dt)
{
  double dx2, dy2;
  int nx, ny;
  double *currdata, *prevdata;

  /* HINT: to help the compiler do not access members of structures
   * within OpenACC parallel regions */
  currdata = curr->data;
  prevdata = prev->data;
  nx = curr->nx;
  ny = curr->ny;

  /* Determine the temperature field at next time step
   * As we have fixed boundary conditions, the outermost gridpoints
   * are not updated. */
  dx2 = prev->dx * prev->dx;
  dy2 = prev->dy * prev->dy;

  #pragma omp parallel shared(nx, ny, a, dt, dx2, dy2, prevdata, currdata)
  {
    #pragma omp for
    for (int i = 2; i < nx; i++)
      for (int j = 2; j < ny; j++)
        five_point_stencil(&i, &j, &ny, &a, &dt, &dx2, &dy2, prevdata, currdata);
  }
}


// calculate temperature around the border
void evolve_boundary(field *curr, field *prev, double a, double dt)
{
  double dx2, dy2;
  int nx, ny;
  double *currdata, *prevdata;

  /* HINT: to help the compiler do not access members of structures
   * within OpenACC parallel regions */
  currdata = curr->data;
  prevdata = prev->data;
  nx = curr->nx;
  ny = curr->ny;

  /* Determine the temperature field at next time step
   * As we have fixed boundary conditions, the outermost gridpoints
   * are not updated. */
  dx2 = prev->dx * prev->dx;
  dy2 = prev->dy * prev->dy;

  #pragma omp parallel shared(nx, ny, a, dt, dx2, dy2, prevdata, currdata)
  {
    #pragma omp for
    for (int i = 1; i < nx + 1; i+=(nx-1))
      for (int j = 1; j < ny + 1; j++)
        five_point_stencil(&i, &j, &ny, &a, &dt, &dx2, &dy2, prevdata, currdata);

    #pragma omp for
    for (int j = 1; j < ny + 1; j+=(ny-1))
      for (int i = 1; i < nx + 1; i++)
        five_point_stencil(&i, &j, &ny, &a, &dt, &dx2, &dy2, prevdata, currdata);
  }
}
