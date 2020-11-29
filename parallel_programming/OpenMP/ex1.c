#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

typedef int bool;
#define FALSE 0
#define TRUE !FALSE

#define MAX_INT 0x7fffffff

typedef struct ValPos
{
    int val;
    int i;
    int j;
} ValPos;

bool isStrictDiagonalDominant(int const *const array, int n, int threadsNum);
int absDiagonalMax(int const *const array, int n, int threadsNum);
void calculateResultArray(int *resultArray, int const *const array, int n, int threadsNum, int max);
ValPos findMin(int const *const array, int n, int threadsNum);

// custom omp reduction clause for min value with position
#pragma omp declare reduction(min_with_pos : ValPos \
                              : omp_out = omp_in.val < omp_out.val ? omp_in : omp_out) \
    initializer(omp_priv = {MAX_INT, -1, -1})

// translate 2d index to 1d
#define INDEX(i, j, n) ((i) * (n) + (j))

int main()
{
    int *array, *resultArray;
    int n;
    int threadsNum;
    int i, j;

    int max;
    ValPos minValPos;

    printf("Give N: ");
    scanf("%d", &n);

    array = (int *)malloc(n * n * sizeof(int));
    resultArray = (int *)malloc(n * n * sizeof(int));
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            printf("A[%d][%d] = ", i, j);
            scanf("%d", &array[INDEX(i, j, n)]);
        }
    }

    printf("Number of threads: ");
    scanf("%d", &threadsNum);

    omp_set_num_threads(threadsNum);

    if (!isStrictDiagonalDominant(array, n, threadsNum))
    {
        printf("The matrix is not strict diagonal dominant\n");
        exit(0);
    }

    max = absDiagonalMax(array, n, threadsNum);

    calculateResultArray(resultArray, array, n, threadsNum, max);

    minValPos = findMin(resultArray, n, threadsNum);

    printf("\nB = {\n");
    for (i = 0; i < n; i++)
    {
        printf("    ");
        for (j = 0; j < n; j++)
        {
            printf("%5d ", resultArray[INDEX(i, j, n)]);
        }
        putchar('\n');
    }
    printf("}\n");

    printf("\nMin value = %d, at pos = %d, %d\n", minValPos.val, minValPos.i, minValPos.j);

    free(array);
    free(resultArray);
    return 0;
}

bool isStrictDiagonalDominant(int const *const array, int n, int threadsNum)
{
    int i, j;
    int lineSum;
    bool flag = TRUE;

    int chunkSize = n / threadsNum;
    if (chunkSize == 0)
        chunkSize = 1;

    #pragma omp parallel default(none) shared(flag, array, chunkSize, n) private(i, j, lineSum)
    {
        // use dynamic because the operations are not the same for all threads (at some point all will continue)
        #pragma omp for schedule(dynamic, chunkSize)
        for (i = 0; i < n; i++)
        {
            // if a thread has found a line that breaks the strict diagonal dominance
            // condition then skip the rest lines
            if (!flag)
                continue;

            lineSum = 0;
            for (j = 0; j < n; j++)
            {
                // printf("\nthread %d i %d j %d", omp_get_thread_num(), i, j);
                if (i != j)
                    lineSum += abs(array[INDEX(i, j, n)]);
            }

            // no need to lock because the final value will always be FALSE
            // even if 2+ threads try to write at the same time
            if (abs(array[INDEX(i, i, n)]) <= lineSum)
                flag = FALSE;
        }
    }

    return flag;
}

int absDiagonalMax(int const *const array, int n, int threadsNum)
{
    int i, j;
    int max, localMax;

    max = localMax = abs(array[0]);

    // int chunkSize = n / threadsNum;
    // if (chunkSize == 0)
    //     chunkSize = 1;

    // // calculate absolute max manually
    // #pragma omp parallel default(none) shared(n, array, chunkSize, max) private(i, j) firstprivate(localMax)
    // {
    //     #pragma omp for schedule(static, chunkSize)
    //     for (i = 0; i < n; i++)
    //     {
    //         if (abs(array[INDEX(i, i, n)]) > localMax)
    //             localMax = abs(array[INDEX(i, i, n)]);
    //     }

    //     #pragma omp critical (calc_max)
    //     {
    //         if (localMax > max)
    //             max = localMax;
    //     }
    // }

    // calculate max using reduction clause
    #pragma omp parallel for reduction(max: max)
    for (i = 0; i < n; i++)
    {
        if (abs(array[INDEX(i, i, n)]) > max)
            max = abs(array[INDEX(i, i, n)]);
    }

    return max;
}

void calculateResultArray(int *resultArray, int const *const array, int n, int threadsNum, int max)
{
    int i, j;

    int chunkSize = n / threadsNum;
    if (chunkSize == 0)
        chunkSize = 1;

    #pragma omp parallel default(none) shared(n, array, resultArray, chunkSize, max) private(i, j)
    {
        #pragma omp for schedule(static, chunkSize)
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                // printf("thread %d i=%d j=%d\n", omp_get_thread_num(), i , j);
                resultArray[INDEX(i, j, n)] = i == j ? max : (max - abs(array[INDEX(i, j, n)]));
            }
        }
    }
}

ValPos findMin(int const *const array, int n, int threadsNum)
{
    int i, j;
    ValPos min;
    int localMin, minPosI, minPosJ;
    int chunkSize;

    // min.val = array[0];
    // min.i = min.j = 0;

    // // calculate min with custom reduction clause that finds and the position of the min value
    // #pragma omp parallel for reduction(min_with_pos: min)
    // for (i = 0; i < n * n; i++)
    // {
    //     if (array[i] < min.val)
    //     {
    //         min.val = array[i];

    //         // translate 1d index to 2d
    //         min.i = i / n;
    //         min.j = i % n;
    //     }
    // }

    // calculate min without reduction clause (with critical)
    min.val = localMin = array[0];
    min.i = minPosI = 0;
    min.j = minPosJ = 0;

    chunkSize = (n * n) / threadsNum;
    if (chunkSize == 0)
        chunkSize = 1;

    #pragma omp parallel default(none) shared(n, array, chunkSize, min) private(i, j) firstprivate(localMin, minPosI, minPosJ)
    {
        #pragma omp for schedule(static, chunkSize)
        for (i = 0; i < n * n; i++)
        {
            if (array[i] < localMin)
            {
                localMin = array[i];
                minPosI = i / n;
                minPosJ = i % n;
            }
        }

        #pragma omp critical(calc_min)
        {
            if (localMin < min.val)
            {
                min.val = localMin;
                min.i = minPosI;
                min.j = minPosJ;
            }
        }
    }

    return min;
}