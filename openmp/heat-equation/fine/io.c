/* I/O related functions for heat equation solver */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#include "heat.h"
#include "pngwriter.h"


/* Output routine that prints out a picture of the temperature
 * distribution. */
void write_field(field *temperature, int iter, parallel_data *parallel)
{
  char filename[64];
  int width = temperature->ny;
  int height = temperature->nx;

  /* The actual write routine takes only the actual data
   * (without ghost layers) so we need array for that. */
  double *full_data, *tmp_data;

  if (parallel->rank == 0)
    full_data = (double *) malloc((temperature->nx_full * temperature->ny_full) * sizeof(double));

  tmp_data = (double *) malloc((width * height) * sizeof(double));

  /* Copy the inner data */
  for (int i = 0; i < height; i++)
    memcpy(&tmp_data[i * width],
           &temperature->data[(i + 1) * (width + 2) + 1],
           width * sizeof(double));

  /* Receive data from other ranks using Gather */
  MPI_Gather(tmp_data,
             width * height,
             MPI_DOUBLE,
             full_data,
             width * height,
             MPI_DOUBLE,
             0,
             MPI_COMM_WORLD);

  if (parallel->rank == 0) {
    /* Write out the data to a png file */
    sprintf(filename, "%s_%04d.png", "heat", iter);
    printf("Image size h:%d, w:%d\n", height, width);
    save_png(full_data, temperature->nx_full, temperature->ny_full, filename, 'c');
    free(full_data);
  }
  free(tmp_data);
}


/* Read the initial temperature distribution from a file and
 * initialize the temperature fields temperature1 and
 * temperature2 to the same initial state. */
void read_field(field *temperature1, field *temperature2, char *filename, parallel_data *parallel)
{
  FILE *fp;
  int nx, ny, ind;
  double *full_data;
  double *inner_data;

  int nx_local, ny_local, count;

  fp = fopen(filename, "r");
  /* Read the header */
  count = fscanf(fp, "# %d %d \n", &nx, &ny);
  if (count < 2) {
    fprintf(stderr, "Error while reading the input file!\n");
    MPI_Abort(MPI_COMM_WORLD, -1);
  }

  parallel_setup(parallel, nx, ny);
  set_field_dimensions(temperature1, nx, ny, parallel);
  set_field_dimensions(temperature2, nx, ny, parallel);

  /* Allocate arrays */
  allocate_field(temperature1);
  allocate_field(temperature2);

  inner_data = (double *) malloc((temperature1->nx * temperature1->ny) * sizeof(double));

  if (parallel->rank == 0) {
    /* Full array */
    full_data = (double *) malloc((nx * ny) * sizeof(double));

    /* Read the actual data */
    for (int i = 0; i < nx; i++) {
      for (int j = 0; j < ny; j++) {
        ind = i * ny + j;
        count = fscanf(fp, "%lf", &full_data[ind]);
      }
    }
  } else {
    /* Dummy array for full data. Some MPI implementations
    * require that this array is actually allocated... */
    full_data = (double *) malloc(sizeof(double));
  }

  nx_local = temperature1->nx;
  ny_local = temperature1->ny;

  MPI_Scatter(full_data, nx_local * ny, MPI_DOUBLE, inner_data,
              nx_local * ny, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  /* Copy to the array containing also boundaries */
  for (int i = 0; i < nx_local; i++)
    memcpy(&temperature1->data[(i + 1) * (ny_local + 2) + 1], &inner_data[i * ny_local],
           ny * sizeof(double));

  /* Set the boundary values */
  for (int i = 1; i < nx_local + 1; i++) {
    temperature1->data[i * (ny_local + 2)] = temperature1->data[i * (ny_local + 2) + 1];
    temperature1->data[i * (ny_local + 2) + ny + 1] = temperature1->data[i * (ny_local + 2) + ny];
  }
  for (int j = 0; j < ny + 2; j++) {
    temperature1->data[j] = temperature1->data[ny_local + j];
    temperature1->data[(nx_local + 1) * (ny_local + 2) + j] =
            temperature1->data[nx_local * (ny_local + 2) + j];
  }

  copy_field(temperature1, temperature2);

  free(full_data);
  free(inner_data);
  fclose(fp);
}
