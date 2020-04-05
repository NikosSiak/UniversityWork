#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0
#define TAG_CYCLIC_D 1

int modulo(int x, int y) {
    // for positive numbers the modulo and the remainder is the same, but the values change when you have negative values
    // in c the operator % gives the remainder, to get the modulo use the below expresion
    return (x % y + y) % y;
}

void addCD(int *c, int *d, int rows, int cols, int *res);
void multiplyCB(int *c, int *b, int rows, int cols, int *res);
void multiplyAB(int *a, int *b, int n, int *res);
void multiplyCD(int *c, int *d, int rank, int n, int numOfProcesses, int *res);
int menu();

int main(int argc, char **argv) {
    int numOfProcesses, rank;
    int *a, *b, *c, *d;
    int *localA, *localB, *localC, *localD;
    int *scatterCounts, *scatterDispls, *gatherCounts, *gatherDispls, remainingRows, remainingNums, scatterOffset, gatherOffset;
    int n, localN, rows, cols;
    int i, j, ind;
    int *res, *localRes;
    int choice;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        choice = menu();
    }
    MPI_Bcast(&choice, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    while (choice != 0) {
        if (rank == 0) {
            printf("Give n: ");
            scanf("%d", &n);
            if (choice == 1 || choice == 4) {
                printf("-----C-----\n");
                c = malloc(n * n * sizeof(int));
                for (i = 0; i < n; i++) {
                    for (j = 0; j < n; j++) {
                        printf("C[%d][%d] = ", i, j);
                        scanf("%d", c+n*i+j);
                    }
                }

                printf("-----D-----\n");
                d = malloc(n * n * sizeof(int));
                for (i = 0; i < n; i++) {
                    for (j = 0; j < n; j++) {
                        printf("D[%d][%d] = ", i, j);
                        scanf("%d", d+n*i+j);
                    }
                }

                res = malloc(n * n * sizeof(int));
            }
            else if (choice == 2) {
                printf("-----B-----\n");
                b = malloc(n * sizeof(int));
                for (i = 0; i < n; i++) {
                    printf("B[%d] = ", i);
                    scanf("%d", b+i);
                }
                
                printf("-----C-----\n");
                c = malloc(n * n * sizeof(int));
                for (i = 0; i < n; i++) {
                    for (j = 0; j < n; j++) {
                        printf("C[%d][%d] = ", i, j);
                        scanf("%d", c+n*i+j);
                    }
                }

                res = malloc(n * sizeof(int));
            }
            else if (choice == 3) {
                printf("-----A-----\n");
                a = malloc(n * sizeof(int));
                for (i = 0; i < n; i++) {
                    printf("A[%d] = ", i);
                    scanf("%d", a+i);
                }

                printf("-----B-----\n");
                b = malloc(n * sizeof(int));
                for (i = 0; i < n; i++) {
                    printf("B[%d] = ", i);
                    scanf("%d", b+i);
                }

                res = malloc(sizeof(int));
            }
            else if (choice == 5) {
                printf("-----B-----\n");
                b = malloc(n * sizeof(int));
                for (i = 0; i < n; i++) {
                    printf("B[%d] = ", i);
                    scanf("%d", b+i);
                }

                res = malloc(2 * sizeof(int));
            }
        }

        MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        localN = n / numOfProcesses;
        rows = localN;
        cols = n;
    
        if (choice == 1) {
            localC = malloc(rows * cols * sizeof(int));
            localD = malloc(rows * cols * sizeof(int));
            localRes = malloc(rows * cols * sizeof(int));

            MPI_Scatter(c, rows * cols, MPI_INT, localC, rows * cols, MPI_INT, ROOT, MPI_COMM_WORLD);
            MPI_Scatter(d, rows * cols, MPI_INT, localD, rows * cols, MPI_INT, ROOT, MPI_COMM_WORLD);

            addCD(localC, localD, rows, cols, localRes);

            MPI_Gather(localRes, rows * cols, MPI_INT, res, rows * cols, MPI_INT, ROOT, MPI_COMM_WORLD);

            if (rank == 0) {
                printf("-----C(%dx%d) + D(%dx%d)-----\n", n, n, n, n);
                for (i = 0; i < n; i++) {
                    for (j = 0; j < n; j++) {
                        printf("res[%d][%d] = %d\n", i, j, res[i*n+j]);
                    }
                }

                free(c);
                free(d);
                free(res);
            }

            free(localC);
            free(localD);
            free(localRes);
        }
        else if (choice == 2) {
            remainingRows = n % numOfProcesses;
            scatterOffset = 0;
            gatherOffset = 0;

            scatterCounts = malloc(numOfProcesses * sizeof(int));
            scatterDispls = malloc(numOfProcesses * sizeof(int));
            gatherCounts = malloc(numOfProcesses * sizeof(int));
            gatherDispls = malloc(numOfProcesses * sizeof(int));

            for (i = 0; i < numOfProcesses; i++) {
                scatterCounts[i] = (n / numOfProcesses) * n;
                gatherCounts[i] = n / numOfProcesses;
                if (remainingRows > 0) {
                    scatterCounts[i] += n;
                    gatherCounts[i]++;
                    remainingRows--;
                }
                scatterDispls[i] = scatterOffset;
                gatherDispls[i] = gatherOffset;
                scatterOffset += scatterCounts[i];
                gatherOffset += gatherCounts[i];
            }

            if (rank != 0) {
                b = malloc(n * sizeof(int));
            }

            localC = malloc(scatterCounts[rank] * sizeof(int));
            localRes = malloc(scatterCounts[rank] / n * sizeof(int));

            // Send the whole b because all processes need all the values to calculate res
            MPI_Bcast(b, n, MPI_INT, ROOT, MPI_COMM_WORLD);
            MPI_Scatterv(c, scatterCounts, scatterDispls, MPI_INT, localC, scatterCounts[rank], MPI_INT, ROOT, MPI_COMM_WORLD);
            
            multiplyCB(localC, b, scatterCounts[rank] / n, cols, localRes);

            MPI_Gatherv(localRes, gatherCounts[rank], MPI_INT, res, gatherCounts, gatherDispls, MPI_INT, ROOT, MPI_COMM_WORLD);

            if (rank == 0) {
                printf("C(%dx%d) * B(%dx1)\n", n, n, n);
                for (i = 0; i < n; i++) {
                    printf("res[%d] = %d\n", i, res[i]);
                }

                free(c);
                free(res);
            }

            free(b);
            free(localC);
            free(localRes);
            free(scatterCounts);
            free(scatterDispls);
            free(gatherCounts);
            free(gatherDispls);
        }
        else if (choice == 3) {
            localA = malloc(localN * sizeof(int));
            localB = malloc(localN * sizeof(int));
            localRes = malloc(sizeof(int));

            MPI_Scatter(a, localN, MPI_INT, localA, localN, MPI_INT, ROOT, MPI_COMM_WORLD);
            MPI_Scatter(b, localN, MPI_INT, localB, localN, MPI_INT, ROOT, MPI_COMM_WORLD);

            multiplyAB(localA, localB, rows, localRes);

            MPI_Reduce(localRes, res, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);

            if (rank == 0) {
                printf("A(1x%d) * B(%dx1) = %d\n", n, n, *res);

                free(a);
                free(b);
                free(res);
            }

            free(localA);
            free(localB);
            free(localRes);
        }
        else if (choice == 4) {
            localC = malloc(rows * cols * sizeof(int));
            localD = malloc(rows * cols * sizeof(int));
            localRes = calloc(n, sizeof(int));

            MPI_Scatter(c, rows * cols, MPI_INT, localC, rows * cols, MPI_INT, ROOT, MPI_COMM_WORLD);
            MPI_Scatter(d, rows * cols, MPI_INT, localD, rows * cols, MPI_INT, ROOT, MPI_COMM_WORLD);

            multiplyCD(localC, localD, rank, n, numOfProcesses, localRes);

            MPI_Gather(localRes, n, MPI_INT, res, n, MPI_INT, ROOT, MPI_COMM_WORLD);

            if (rank == 0) {
                printf("C(%dx%d) * D(%dx%d)\n", n, n, n, n);
                for (i = 0; i < n; i++) {
                    for (j = 0; j < n; j++) {
                        printf("res[%d][%d] = %d\n", i, j, res[i*cols+j]);
                    }
                }

                free(c);
                free(d);
                free(res);
            }

            free(localC);
            free(localD);
            free(localRes);
        }
        else if (choice == 5) {
            remainingNums = n % numOfProcesses;
            scatterOffset = 0;

            scatterCounts = malloc(numOfProcesses * sizeof(int));
            scatterDispls = malloc(numOfProcesses * sizeof(int));

            for (i = 0; i < numOfProcesses; i++) {
                scatterCounts[i] = n / numOfProcesses;
                if (remainingNums > 0) {
                    scatterCounts[i]++;
                    remainingNums--;
                }
                scatterDispls[i] = scatterOffset;
                scatterOffset += scatterCounts[i];
            }
            localB = malloc(scatterCounts[rank] * sizeof(int));
            localRes = malloc(2 * sizeof(int));

            MPI_Scatterv(b, scatterCounts, scatterDispls, MPI_INT, localB, scatterCounts[rank], MPI_INT, ROOT, MPI_COMM_WORLD);

            // localRes[0] == the number
            // localRes[1] == the index of number in array b
            localRes[0] = localB[0];
            localRes[1] = scatterDispls[rank];

            for (i = 1; i < scatterCounts[rank]; i++) {
                if (localRes[0] < localB[i]) {
                    localRes[0] = localB[i];
                    localRes[1] = scatterDispls[rank] + i;
                }
            }

            MPI_Reduce(localRes, res, 1, MPI_2INT, MPI_MAXLOC, ROOT, MPI_COMM_WORLD);

            if (rank == 0) {
                printf("Max of B = %d, in position %d\n", res[0], res[1]);

                free(b);
                free(res);
            }

            free(localB);
            free(localRes);
            free(scatterCounts);
            free(scatterDispls);
        }

        if (rank == 0) {
            choice = menu();
        }
        MPI_Bcast(&choice, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

void addCD(int *c, int *d, int rows, int cols, int *res) {
    int i;
    for (i = 0; i < rows * cols; i++) {
        res[i] = c[i] + d[i];
    }
}

void multiplyCB(int *c, int *b, int rows, int cols, int *res) {
    int rowSum, i, j;
    for (i = 0; i < rows; i++) {
        rowSum = 0;
        for (j = 0; j < cols; j++) {
            rowSum += c[i * cols + j] * b[j];
        }
        res[i] = rowSum;
    }
}

void multiplyAB(int *a, int *b, int n, int *res) {
    int i;
    *res = 0;
    for (i = 0; i < n; i++) {
        (*res) += (a[i] * b[i]);
    }
}

void multiplyCD(int *c, int *d, int rank, int n, int numOfProcesses, int *res) {
    int i, j, ind;
    ind = rank;
    for (i = 0; i < numOfProcesses; i++) {
        for (j = 0; j < n; j++) {
            res[j] += (c[ind] * d[j]);
        }
        MPI_Send(d, n, MPI_INT, modulo(rank+1, numOfProcesses), TAG_CYCLIC_D, MPI_COMM_WORLD);
        MPI_Recv(d, n, MPI_INT, modulo(rank-1, numOfProcesses), TAG_CYCLIC_D, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        ind = modulo(ind-1, n);
    }
}

int menu() {
    int choice;
    printf("-----MENU-----\n");
    printf("0) exit\n");
    printf("1) C + D\n");
    printf("2) C * B\n");
    printf("3) A * B\n");
    printf("4) C * D\n");
    printf("5) max(B)\n");
    printf("> ");
    scanf("%d", &choice);
    return choice;
}
