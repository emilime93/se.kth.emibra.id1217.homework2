/* matrix summation using OpenMP
    usage with gcc (version 4.2 or higher required):
    gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c 
    ./matrixSum-openmp size numWorkers

*/

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#define MAXSIZE 10000 /* maximum matrix size */
#define MAXWORKERS 8  /* maximum number of workers */

typedef struct {
    long value;
    long i;
    long j;
} Index;

int numWorkers;
int size;
int matrix[MAXSIZE][MAXSIZE];

/* read command line, initialize, and create threads */
int main(int argc, char *argv[])
{
    int i, j, total = 0;
    Index max_index, min_index;
    max_index.value = LONG_MIN;
    min_index.value = LONG_MAX;
    int max_val = INT_MIN, min_val = INT_MAX;

    /* read command line args if any */
    size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
    if (size > MAXSIZE)
        size = MAXSIZE;
    if (numWorkers > MAXWORKERS)
        numWorkers = MAXWORKERS;

    omp_set_num_threads(numWorkers);

    /* initialize the matrix */
    srand(time(NULL));
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            matrix[i][j] = rand() % 99;
        }
    }

#ifdef DEBUG
    for (i = 0; i < size; i++) {
        printf("[");
        for (j = 0; j < size; j++) {
            printf("%*d", 4, matrix[i][j]);
        }
        printf(" ]\n");
    }
#endif

    double start_time = omp_get_wtime();

    #pragma omp parallel for reduction(+ : total), reduction(max : max_val), reduction(min : min_val) private(j)
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++) {
            total += matrix[i][j];
            if (matrix[i][j] > max_val) {
                max_val = matrix[i][j];
                #pragma omp critical
                {
                    if (max_val > max_index.value) {
                        max_index.value = max_val;
                        max_index.i = i;
                        max_index.j = j;
                    }
                }
            }
            if (matrix[i][j] < min_val) {
                min_val = matrix[i][j];
                #pragma omp critical
                {
                    if (min_val < min_index.value) {
                        min_index.value = mai_val;
                        min_index.i = i;
                        min_index.j = j;
                    }
                }
            }
        }
    
    
    // implicit barrier

    double end_time = omp_get_wtime();

    printf("the total is %d\n", total);
    printf("Max: %ld (%ld, %ld)\n", max_index.value, max_index.i, max_index.j);
    printf("Min: %ld (%ld, %ld)\n", min_index.value, min_index.i, min_index.j);
    printf("it took %g seconds\n", end_time - start_time);
}
