#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define ROOT 0

#define ALIVE 1
#define DEAD 0

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define LEFT_UP 4
#define RIGHT_UP 5
#define LEFT_DOWN 6
#define RIGHT_DOWN 7

void printWorld(int *a, int n);

int main(int argc, char **argv) {
    int numOfProcesses, rankWorld, rankHive;
    int *a, localA, nbrsA[8], n, emulationCycles;
    int aliveCount = 0;
    int diedAloneCount, diedOverpopulatedCount, bornedCount;
    int localDiedAloneCount, localDiedOverpopulatedCount, localBornedCount;
    int nbrsAliveCount;
    int i, j, k;

    int ndims = 2;              // 2d matrix
    int reorder = 1;            // allow reorder
    int periods[2] = {0, 0};    // [0] rows non-periodic, [1] columns non-periodic
    int dimSize[2];
    MPI_Comm hiveComm;
    MPI_Request reqs[16];

    int nbrs[8];
    
    int coords[2];
    int nbrsCoords[2];

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rankWorld);

    if (rankWorld == 0) {
        printf("Give n: ");
        scanf("%d", &n);
        printf("Give the number of cycles to emulate: ");
        scanf("%d", &emulationCycles);
    }

    MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&emulationCycles, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    dimSize[0] = n; // rows
    dimSize[1] = n; // columns

    MPI_Cart_create(MPI_COMM_WORLD, ndims, dimSize, periods, reorder, &hiveComm);
    MPI_Comm_rank(hiveComm, &rankHive);

    if (rankHive == ROOT) {
        srand(time(NULL));
        a = malloc(n * n * sizeof(int));

        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                a[i * n + j] = rand() % 2;
                // printf("a[%d][%d] = ", i, j);
                // scanf("%d", a + i * n + j);

                if (a[i * n + j] == ALIVE) {
                    aliveCount++;
                }
            }
        }

        printWorld(a, n);
        printf("Press enter to continue...");
        getchar();
        getchar();
    }

    MPI_Cart_coords(hiveComm, rankHive, ndims, coords);
    MPI_Cart_shift(hiveComm, 0, 1, &nbrs[UP], &nbrs[DOWN]);
    MPI_Cart_shift(hiveComm, 1, 1, &nbrs[LEFT], &nbrs[RIGHT]);

    nbrsCoords[0] = coords[0] - 1;
    nbrsCoords[1] = coords[1] - 1; // left up
    if (nbrsCoords[0] < n && nbrsCoords[0] > -1 && nbrsCoords[1] < n && nbrsCoords[1] > -1) {
        MPI_Cart_rank(hiveComm, nbrsCoords, &nbrs[LEFT_UP]);
    }
    else {
        nbrs[LEFT_UP] = -1;
    }

    nbrsCoords[0] = coords[0] - 1;
    nbrsCoords[1] = coords[1] + 1; // right up
    if (nbrsCoords[0] < n && nbrsCoords[0] > -1 && nbrsCoords[1] < n && nbrsCoords[1] > -1) {
        MPI_Cart_rank(hiveComm, nbrsCoords, &nbrs[RIGHT_UP]);
    }
    else {
        nbrs[RIGHT_UP] = -1;
    }

    nbrsCoords[0] = coords[0] + 1;
    nbrsCoords[1] = coords[1] - 1; // left down
    if (nbrsCoords[0] < n && nbrsCoords[0] > -1 && nbrsCoords[1] < n && nbrsCoords[1] > -1) {
        MPI_Cart_rank(hiveComm, nbrsCoords, &nbrs[LEFT_DOWN]);
    }
    else {
        nbrs[LEFT_DOWN] = -1;
    }

    nbrsCoords[0] = coords[0] + 1;
    nbrsCoords[1] = coords[1] + 1; // right down
    if (nbrsCoords[0] < n && nbrsCoords[0] > -1 && nbrsCoords[1] < n && nbrsCoords[1] > -1) {
        MPI_Cart_rank(hiveComm, nbrsCoords, &nbrs[RIGHT_DOWN]);
    }
    else {
        nbrs[RIGHT_DOWN] = -1;
    }

    MPI_Scatter(a, 1, MPI_INT, &localA, 1, MPI_INT, ROOT, hiveComm);

    for (i = 0; i < emulationCycles; i++) {
        localDiedAloneCount = 0;
        localDiedOverpopulatedCount = 0;
        localBornedCount = 0;
        nbrsAliveCount = 0;
        for (j = 0; j < 8; j++) {
            MPI_Isend(&localA, 1, MPI_INT, nbrs[j], 10, hiveComm, &reqs[j]);
            MPI_Irecv(&nbrsA[j], 1, MPI_INT, nbrs[j], 10, hiveComm, &reqs[j + 8]);
        }

        MPI_Waitall(16, reqs, MPI_STATUS_IGNORE);
        for (j = 0; j < 8; j++) {
            if (nbrs[j] > -1 && nbrsA[j] == ALIVE) {
                nbrsAliveCount++;
            }
        }

        if (localA == ALIVE) {
            if (nbrsAliveCount < 2) {
                localDiedAloneCount++;
                localA = DEAD;
            }
            else if (nbrsAliveCount > 3) {
                localDiedOverpopulatedCount++;
                localA = DEAD;
            }
        }
        else if (localA == DEAD && nbrsAliveCount == 3) {
            localBornedCount++;
            localA = ALIVE;
        }

        MPI_Gather(&localA, 1, MPI_INT, a, 1, MPI_INT, ROOT, hiveComm);
        MPI_Reduce(&localDiedAloneCount, &diedAloneCount, 1, MPI_INT, MPI_SUM, ROOT, hiveComm);
        MPI_Reduce(&localDiedOverpopulatedCount, &diedOverpopulatedCount, 1, MPI_INT, MPI_SUM, ROOT, hiveComm);
        MPI_Reduce(&localBornedCount, &bornedCount, 1, MPI_INT, MPI_SUM, ROOT, hiveComm);

        if (rankHive == ROOT) {
            aliveCount -= diedAloneCount;
            aliveCount -= diedOverpopulatedCount;
            aliveCount += bornedCount;
            printf("-----Generation %d-----\n", i+1);
            printWorld(a, n);
            printf("Alive = %d\n", aliveCount);
            printf("Died from lonely = %d\n", diedAloneCount);
            printf("Died from overpopulation = %d\n", diedOverpopulatedCount);
            printf("Borned = %d\n", bornedCount);
            printf("Press enter to continue...");
            getchar();
        }
    }

    if (rankHive == ROOT) {
        free(a);
    }


    MPI_Finalize();
    return 0;
}

void printWorld(int *a, int n) {
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (a[i * n + j] == ALIVE) {
                printf("| A ");
            }
            else {
                printf("|   ");
            }
        }
        printf("|\n");
    }
}

