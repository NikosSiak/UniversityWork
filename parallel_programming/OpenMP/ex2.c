#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

void multisort(int *array, int *space, int n);

int main()
{
    int *array, *space;
    int n;
    int threadsNum;
    int i;

    printf("Give N: ");
    scanf("%d", &n);

    array = (int *)malloc(n * sizeof(int));
    space = (int *)malloc(n * sizeof(int));
    for (i = 0; i < n; i++)
    {
        printf("A[%d] = ", i);
        scanf("%d", &array[i]);
    }

    printf("Number of threads: ");
    scanf("%d", &threadsNum);

    omp_set_num_threads(threadsNum);

    #pragma omp parallel
        #pragma omp single
            multisort(array, space, n);

    printf("Sorted array: [ ");
    for (i = 0; i < n; i++)
    {
        printf("%d ", array[i]);
    }
    printf("]\n");

    return 0;
}

int cmpfunc (const void * a, const void * b)
{
    return (*(int *)a - *(int *)b);
}

void merge(int *a, int *endA, int *b, int *endB, int *res)
{
    int i = 0;

    while (a <= endA && b <= endB)
    {
        if ((*a) < (*b))
        {
            res[i++] = *a;
            a++;
        }
        else
        {
            res[i++] = *b;
            b++;
        }
    }

    while (a <= endA)
    {
        res[i++] = *a;
        a++;
    }

    while (b <= endB)
    {
        res[i++] = *b;
        b++;
    }
}

void multisort(int *array, int *space, int n)
{
    int quarter = n / 4;

    if (quarter < 4)
    {
        qsort(array, n, sizeof(int), cmpfunc);
        return;
    }

    int *startA = array;            int *spaceA = space;
    int *startB = startA + quarter; int *spaceB = spaceA + quarter;
    int *startC = startB + quarter; int *spaceC = spaceB + quarter;
    int *startD = startC + quarter; int *spaceD = spaceC + quarter;

    #pragma omp task
    multisort(startA, spaceA, quarter);

    #pragma omp task
    multisort(startB, spaceB, quarter);

    #pragma omp task
    multisort(startC, spaceC, quarter);
    
    #pragma omp task
    multisort(startD, spaceD, n - 3 * quarter);

    #pragma omp taskwait

    #pragma omp task
    merge(startA, startA + quarter - 1, startB, startB + quarter - 1, spaceA);

    #pragma omp task
    merge(startC, startC + quarter - 1, startD, array + n - 1, spaceC);

    #pragma omp taskwait

    merge(spaceA, spaceC - 1, spaceC, space + n - 1, array);
}
