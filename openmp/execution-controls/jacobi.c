#include <omp.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
  int nx;
  int ny;
  double* data;
} matrix;

void read_file(matrix* mat);
double wtime();


// Solves Poisson equation with Jacobi iteration
// Source b is read from file "input.dat"
int main()
{
  const double eps = 0.005;
  matrix u, unew, b;
  int nx, ny;

  double norm, *norms;
  int iter = 0;

  double t_start = wtime();

  #pragma omp parallel
  {
    #pragma omp single
    {
      // Read b
      read_file(&b);

      nx = b.nx;
      ny = b.ny;

      // Allocate space also for boundaries
      u.data = (double*) malloc((nx+2)*(ny+2) * sizeof(double));
      unew.data = (double*) malloc((nx+2)*(ny+2) * sizeof(double));
    }

    // Initialize
    #pragma omp for
    for (int i=0; i < nx+2; i++)
      for (int j=0; j < ny+2; j++) {
        int ind = (ny+2)*i + j;
        u.data[ind] = 0.0;
      }

    #pragma omp single
    memcpy(unew.data, u.data, (nx+2)*(ny+2)*sizeof(double));

    // Jacobi iteration
    do {
      #pragma omp barrier
      #pragma omp single
      norm = 0.0;

      #pragma omp for reduction(+:norm)
      for (int i=1; i < nx+1; i++)
        for (int j=1; j < ny+1; j++) {
          int bcur = ny*(i-1) + (j-1);// (i-1, j-1) b does not contain boundaries
          int ul = (ny+2)*i + (j-1);  // (i,   j-1)
          int ur = (ny+2)*i + (j+1);  // (i,   j+1)
          int uu = ((ny+2)-1)*i + j;  // (i-1, j)
          int ud = ((ny+2)+1)*i + j;  // (i+1, j)
          int ucur = (ny+2)*i + j;    // (i,   j)
          unew.data[ucur] = 0.25 * (u.data[ul] + u.data[ur] +
                                    u.data[uu] + u.data[ud] -
                                    b.data[bcur]);
          norm += (unew.data[ucur] - u.data[ucur]) * (unew.data[ucur] - u.data[ucur]);
        }

      #pragma omp single
      {
        double* temp = u.data;
        u.data = unew.data;
        unew.data = temp;

        if (iter % 500 == 0)
          printf("Iteration %d norm: %lf\n", iter, norm);
          fflush(stdout);
        iter++;
      }
    } while (norm > eps);
  } // end parallel

  double t_end = wtime();

  printf("Converged in %d iterations, norm %lf\nTime spent %lf\n", iter, norm, t_end-t_start);
  fflush(stdout);
}


void read_file(matrix* mat)
{
  FILE* fp;
  fp = fopen("input.dat", "r");
  if (!fp) {
    printf("Couldn't open the file 'input.dat'\n");
    exit(-1);
  }

  /* Read the header */
  int count = fscanf(fp, "# %d %d \n", &(mat->nx), &(mat->ny));

  if (count < 2) {
    printf("Error while reading the input file!\n");
    exit(-1);
  }

  // Read the header
  mat->data = (double*) malloc(mat->nx * mat->ny * sizeof(double));

  /* Read the actual data */
  for (int i = 0; i < mat->nx; i++) {
    for (int j = 0; j < mat->ny; j++) {
      int ind = i * mat->ny + j;
      count = fscanf(fp, "%lf", mat->data+ind);
    }
  }

  fclose(fp);
}


double wtime() {
  return omp_get_wtime();
}
