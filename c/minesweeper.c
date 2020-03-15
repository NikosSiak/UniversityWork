#include <stdio.h>
#include <time.h>
#include <stdlib.h>

char **createTable(int M, int N);
void placeMines(char **field, int M, int N, int K);
void placeNumbers(char **field, int M, int N);
void printTable(char **field, int M, int N, char **opened);
void writeFile(char **field, int M, int N);
int cell(char **field, int M, int N, int K, char **opened);

int main() {
    int M, N, K;
    char **field, **opened;
    do {
        printf("Give rows, columns and bombs: ");
        scanf("%d %d %d", &M, &N, &K);
    } while(M < 1 || N < 1);
    field = createTable(M, N);
    opened = createTable(M, N);
    placeMines(field, M, N, K);
    placeNumbers(field, M, N);
    printTable(field, M, N, opened);
    while(cell(field, M, N, K, opened));
    writeFile(field, M, N);
    return 0;
}

char **createTable(int M, int N) {
    int i, j;
    char **arr;
    arr = (char **) malloc((M+2) * sizeof(char *));
    for (i = 0; i < M+2; i++)
        arr[i] = (char *) malloc((N+2));
    for (i = 0; i < M+2; i++)
        for (j = 0; j < N+2; j++)
            arr[i][j] = '0';
    return arr;
}

void placeMines(char **field, int M, int N, int K) {
    srand(time(NULL));
    int i, r1, r2;
    for (i = 0; i < K; i++) {
        do {
            r1 = rand() % (M+1);
            r2 = rand() % (N+1);
        }
        while (r1 == 0 || r2 == 0 || field[r1][r2] == '*');
        field[r1][r2] = '*';
    }
}

void placeNumbers(char **field, int M, int N) {
    int i, j;
    for (i = 1; i < M+1; i++)
        for (j = 1; j < N+1; j++)
            if (field[i][j] == '*') {
                if (field[i][j-1] != '*')
                    field[i][j-1] = (int) field[i][j-1] + 1;
                if (field[i][j+1] != '*')
                    field[i][j+1] = (int) field[i][j+1] + 1;
                if (field[i-1][j] != '*')
                    field[i-1][j] = (int) field[i-1][j] + 1;
                if (field[i+1][j] != '*')
                    field[i+1][j] = (int) field[i+1][j] + 1;
                if (field[i+1][j+1] != '*')
                    field[i+1][j+1] = (int) field[i+1][j+1] + 1;
                if (field[i-1][j-1] != '*')
                    field[i-1][j-1] = (int) field[i-1][j-1] + 1;
                if (field[i+1][j-1] != '*')
                    field[i+1][j-1] = (int) field[i+1][j-1] + 1;
                if (field[i-1][j+1] != '*')
                    field[i-1][j+1] = (int) field[i-1][j+1] + 1;
            }
}

void printTable(char **field, int M, int N, char **opened) {
    int i, j;
    system("cls");
    printf("  ");
    for (i = 1; i <= N; i++)
        printf("%2d ", i);
    putchar('\n');
    for (i = 1; i <= M; i++) {
        printf("%2d", i);
        for (j = 1; j <= N; j++) {
            if (opened[i][j] == '1')
                printf("[%c]", field[i][j]);
            else
                printf("[ ]");
        }
        putchar('\n');
    }
}

void writeFile(char **field, int M, int N) {
    FILE *fmines;
    int i, j;
    fmines = fopen("field.txt", "w");
    for(i = 1; i < M+1; i++) {
        for(j = 1; j < N+1; j++)
            fprintf(fmines, "%c ", field[i][j]);
        fprintf(fmines, "\n");
    }
    fclose(fmines);
}

int cell(char **field, int M, int N, int K, char **opened) {
    int i, j, line, col;
    static int end = 0;
    do {
        printf("Pick a cell(row column): ");
        scanf("%d %d", &line, &col);
    } while((line > M || line < 1) || (col > N || col < 1) || opened[line][col] == '1');
    opened[line][col] = '1';
    end++;
    if (line+1 <= M && field[line+1][col] != '*' && opened[line+1][col] == '0') {
        opened[line+1][col] = '1';
        end++;
    }
    if (line-1 > 0 && field[line-1][col] != '*' && opened[line-1][col] == '0') {
        opened[line-1][col] = '1';
        end++;
    }
    if (col+1 <= N && field[line][col+1] != '*' && opened[line][col+1] == '0') {
        opened[line][col+1] = '1';
        end++;
    }
    if (col-1 > 0 && field[line][col-1] != '*' && opened[line][col-1] == '0') {
        opened[line][col-1] = '1';
        end++;
    }
    if (line+1 <= M && col+1 <= N && field[line+1][col+1] != '*' && opened[line+1][col+1] == '0') {
        opened[line+1][col+1] = '1';
        end++;
    }
    if (line+1 <= M && col-1 > 0 && field[line+1][col-1] != '*' && opened[line+1][col-1] == '0') {
        opened[line+1][col-1] = '1';
        end++;
    }
    if (line-1 > 0 && col+1 <= N && field[line-1][col+1] != '*' && opened[line-1][col+1] == '0') {
        opened[line-1][col+1] = '1';
        end++;
    }
    if (line-1 > 0 && col-1 > 0 && field[line-1][col-1] != '*' && opened[line-1][col-1] == '0') {
        opened[line-1][col-1] = '1';
        end++;
    }
    printTable(field, M, N, opened);
    if (field[line][col] == '*') {
        printf("Game Over");
        return 0;
    }
    else if (end == M * N - K) {
        printf("You Won");
        return 0;
    }
    else
        return 1;
}
