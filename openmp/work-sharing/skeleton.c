#include <stdio.h>
#include <omp.h>

#define NX 102400

int main(void)
{
    double vecA[NX], vecB[NX], vecC[NX];
    double sum = 0.0;

    /* Initialization of the vectors */
    for (int i = 0; i < NX; i++) {
        vecA[i] = 1.0 / ((double)(NX - i));
        vecB[i] = vecA[i] * vecA[i];
    }

    /*   Implement here a parallelized version of vector addition,
     *   vecC = vecA + vecB
     */
    #pragma omp parallel
    {
        int nthreads = omp_get_num_threads();
        double* sums = (double*) malloc(nthreads * sizeof(double));
        for (int i = 0; i < nthreads; i++) {
            sums[i] = 0.0;
        }
        int ind = omp_get_thread_num();

        #pragma omp for
        {
            /* Compute the check value */
            for (int i = 0; i < NX; i++) {
                vecC[i] = vecA[i] + vecB[i];
                sums[ind] += vecC[i];
            }
        }
        for (int i = 0; i < nthreads; i++) {
            sum += sums[i];
        }

        free(sums);
    }

    printf("Reduction sum: %18.16f\n", sum);

    return 0;
}
