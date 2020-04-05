#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_SIZE 1
#define TAG_NUMS 2
#define TAG_SUM 3
#define TAG_AVG 4
#define TAG_DISP 5
#define TAG_LOCAL_MIN 6
#define TAG_LOCAL_MAX 7
#define TAG_GLOBAL_MIN 8
#define TAG_GLOBAL_MAX 9
#define TAG_D 10
#define TAG_MENU 11

float square(float num) {
    return num * num;
}

int main(int argc, char **argv) {
    int numOfProcesses, rank;
    int i, n, remainingNums, *numbers;
    int sizeOfBuffer, offset;
    int localSum, sum;
    float avg, localDispersionSum, dispersionSum, dispersion;
    int localMin, localMax, min, max;
    float *d;
    int ch;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    do {
        localSum = 0;
        sum = 0;
        localDispersionSum = 0;
        dispersionSum = 0;
        
        // allocate memory and send nums
        if (rank == 0) {
            printf("Give n: ");
            scanf("%d", &n);

            numbers = malloc(n * sizeof(int));
            d = malloc(n * sizeof(float));
            for (i = 0; i < n; i++) {
                printf("x[%d] = ", i);
                scanf("%d", numbers+i);
            }

            sizeOfBuffer = n / numOfProcesses;
            remainingNums = n % numOfProcesses;

            for (i = 1; i < numOfProcesses; i++) {
                sizeOfBuffer = n / numOfProcesses;
                offset = i * sizeOfBuffer;
                if (i < remainingNums) {
                    sizeOfBuffer++;
                }
                offset += i - 1 < remainingNums ? i : remainingNums;
                MPI_Send(&sizeOfBuffer, 1, MPI_INT, i, TAG_SIZE, MPI_COMM_WORLD); 
                MPI_Send(numbers + offset, sizeOfBuffer, MPI_INT, i, TAG_NUMS, MPI_COMM_WORLD);
            }

            sizeOfBuffer = n / numOfProcesses;
            if (remainingNums != 0) {
                sizeOfBuffer++;
            }
        }
        else {
            MPI_Recv(&sizeOfBuffer, 1, MPI_INT, 0, TAG_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            numbers = malloc(sizeOfBuffer * sizeof(int));
            d = malloc(sizeOfBuffer * sizeof(float));
            MPI_Recv(numbers, sizeOfBuffer, MPI_INT, 0, TAG_NUMS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // find min, max, sum of each process
        localMin = numbers[0];
        localMax = numbers[0];

        // if this process didn't have any numbers localMin/localMax would have garbage
        // so if i assign them to the max/min int respectively their values would be ignored in the process 0
        // when it compares them to the other numbers 
        if (sizeOfBuffer == 0) {
            localMin = INT32_MAX;
            localMax = INT32_MIN;
        }

        for (i = 0; i < sizeOfBuffer; i++) {
            localSum += numbers[i];
            if (numbers[i] < localMin) {
                localMin = numbers[i];
            }
            else if (numbers[i] > localMax) {
                localMax = numbers[i];
            }
        }

        // recv all local min, max, sum and send the global ones
        if (rank == 0) {
            sum = localSum;
            min = localMin;
            max = localMax;
            for (i = 1; i < numOfProcesses; i++) {
                MPI_Recv(&localSum, 1, MPI_INT, i, TAG_SUM, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                sum += localSum;

                MPI_Recv(&localMin, 1, MPI_INT, i, TAG_LOCAL_MIN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&localMax, 1, MPI_INT, i, TAG_LOCAL_MAX, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (localMin < min) {
                    min = localMin;
                }
                if (localMax > max) {
                    max = localMax;
                }
            }

            avg = sum / (float)n;
            printf("\n\nAverage = %f\n", avg);

            for (i = 1; i < numOfProcesses; i++) {
                MPI_Send(&avg, 1, MPI_FLOAT, i, TAG_AVG, MPI_COMM_WORLD);
                MPI_Send(&min, 1, MPI_INT, i, TAG_GLOBAL_MIN, MPI_COMM_WORLD);
                MPI_Send(&max, 1, MPI_INT, i, TAG_GLOBAL_MAX, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Send(&localSum, 1, MPI_INT, 0, TAG_SUM, MPI_COMM_WORLD);
            MPI_Send(&localMin, 1, MPI_INT, 0, TAG_LOCAL_MIN, MPI_COMM_WORLD);
            MPI_Send(&localMax, 1, MPI_INT, 0, TAG_LOCAL_MAX, MPI_COMM_WORLD);

            MPI_Recv(&avg, 1, MPI_FLOAT, 0, TAG_AVG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&min, 1, MPI_INT, 0, TAG_GLOBAL_MIN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&max, 1, MPI_INT, 0, TAG_GLOBAL_MAX, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // find dispersion
        for (i = 0; i < sizeOfBuffer; i++) {
            localDispersionSum += square(numbers[i] - avg);
        }

        // recv all localDispersionSum
        if (rank == 0) {
            dispersionSum = localDispersionSum;
            for (i = 1; i < numOfProcesses; i++) {
                MPI_Recv(&localDispersionSum, 1, MPI_FLOAT, i, TAG_DISP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                dispersionSum += localDispersionSum;
            }
            dispersion = dispersionSum / n;
            printf("Dispersion = %f\n\n\n", dispersion);
        }
        else {
            MPI_Send(&localDispersionSum, 1, MPI_FLOAT, 0, TAG_DISP, MPI_COMM_WORLD);
        }

        for (i = 0; i < sizeOfBuffer; i++) {
            d[i] = ((numbers[i] - min) / (float)(max - min)) * 100;
        }

        // send/recv all d arrays
        if (rank == 0) {
            for (i = 1; i < numOfProcesses; i++) {
                sizeOfBuffer = n / numOfProcesses;
                offset = i * sizeOfBuffer;
                if (i < remainingNums) {
                    sizeOfBuffer++;
                }
                offset += i - 1 < remainingNums ? i : remainingNums;
                MPI_Recv(d + offset, sizeOfBuffer, MPI_FLOAT, i, TAG_D, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            for (i = 0; i < n; i++) {
                printf("d[%d] = %f\n", i, d[i]);
            }
        }
        else {
            MPI_Send(d, sizeOfBuffer, MPI_FLOAT, 0, TAG_D, MPI_COMM_WORLD);
        }

        free(d);
        free(numbers);

        if (rank == 0) {
            printf("\n\n");
            printf("1) Continue\n");
            printf("2) Exit\n");
            printf("> ");
            scanf("%d", &ch);
            for (i = 1; i < numOfProcesses; i++) {
                MPI_Send(&ch, 1, MPI_INT, i, TAG_MENU, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&ch, 1, MPI_INT, 0, TAG_MENU, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } while (ch == 1);

    MPI_Finalize();

    return 0;
}
