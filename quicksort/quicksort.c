#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <omp.h>

#define MAXSIZE 100000000
#define MAXWORKERS 8
#define THRESHOLD 10

void quicksort(int *, long, long);
void insertion_sort(int *, long, long);
int partition(int *, long, long);
void swap(int *, int *);

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void insertion_sort(int *arr, long lo, long hi) {
    int i;
    for (i = lo+1; i < hi; i++) {
        int ele = i;
        while ( (arr[ele-1] > arr[ele]) && ele > 0) {
            swap(&arr[ele-1], &arr[ele]);
            ele--;
        }
    }
}

void quicksort(int *arr, long lo, long hi) {
    if (hi - lo <= THRESHOLD) {
        insertion_sort(arr, lo, hi);
    }
    if (lo < hi) {
        long pivot_location = partition(arr, lo, hi);
        #pragma omp task
        {
            quicksort(arr, lo, pivot_location);   
        }
        #pragma omp task
        {
            quicksort(arr, pivot_location+1, hi);
        }
    }
}

int partition(int *arr, long lo, long hi) {
    int pivot = arr[lo];
    long i = lo - 1;
    long j = hi + 1;
    while (1) {
        i++;
        while (i < hi && arr[i] < pivot)
            i++;
        j--;
        while (j > lo && arr[j] > pivot)
            j--;

        if(i < j) 
            swap(&arr[i], &arr[j]);
        else
            return j;
    }
}

int main(int argc, char **argv) {
    /* Argument initializing */
    long problem_size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
    int num_workers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
    problem_size = (problem_size > MAXSIZE) ? MAXSIZE : problem_size;
    num_workers = (num_workers > MAXWORKERS) ? MAXWORKERS : num_workers;

    printf("\n=== RUN INFO ===\nProblem size: %d\nNum Workers: %d\n\n", problem_size, num_workers);

    int *arr = malloc(problem_size * sizeof(int));

    /* Give a pretty random seed for every run */
    srand(time(NULL));
    long i;
    for (i = 0; i < problem_size; i++) {
        arr[i] = rand() % 100;
    }

    #ifdef DEBUG
        printf("Unsorted array:\n");
        for (i = 0; i < problem_size; i++) {
            printf("%d\n", arr[i]);
        }
    #endif

    /* Set desired number of threads */
    omp_set_num_threads(num_workers);

    double start_time = omp_get_wtime();

    /**
     * Start a parallel region for the recussion work, but restrict the first
     * call to only one thread, to that the work is not done multiple times.
     */
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            quicksort(arr, 0, problem_size);
        }
    }

    double end_time = omp_get_wtime();

    #ifdef DEBUG
        printf("\nAfter sort\n");
        for (int i = 0; i < problem_size; i++) {
            printf("%d\n", arr[i]);
        }
    #endif

    /* Free up the allocated memory */
    free(arr);

    printf("---> It took %f seconds\n", end_time-start_time);
    return 0;
}