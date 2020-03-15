#include <stdio.h>
#include <stdlib.h>

typedef struct rec {
    char ln;
    char col;
    char val;
} rec;

void printTable(char **puzzle);
int validNum(char **puzzle, int num, int r, int c);
int findPosition(char **puzzle, int *i, int *j, int skip);
int findValue(char **puzzle, int m);
int findValueRec(char **puzzle, rec *tel);
int missingNums(char **puzzle, char *missingValues, int r, int c);

int main(int argc, char *argv[]) {
    FILE *fp;
    rec *puzzleStruct, *tel;
    char **puzzle;
    int k, i, j;
    
    if (argc != 2) {
        printf("Usage: %s file_name\n", argv[0]);
        return 1;
    }

    if ((fp = fopen(argv[1], "rb")) == NULL) {
        printf("Couldn't open file\n");
        return 2;
    }

    puzzleStruct = (struct rec *) malloc(81 * sizeof(rec));
    k = fread(puzzleStruct, 3, 81, fp);
    fseek(fp, 0, SEEK_END);
    if (k * 3 != ftell(fp)) {
        printf("error reading puzzle\n");
        return 3;
    }

    puzzle = (char **) malloc(9 * sizeof(char *));
    for (i = 0; i < 9; i++)
        puzzle[i] = (char *) malloc(9);
    for (i = 0; i < 9; i++)
        for (j = 0; j < 9; j++)
            puzzle[i][j] = '0';
    
    for (i = 0; i < k; i++)
        puzzle[(*(puzzleStruct+i)).ln-1][(*(puzzleStruct+i)).col-1] = (*(puzzleStruct+i)).val+48;
    
    k = 81 - k;
    printTable(puzzle);
    while (findValue(puzzle, k)) {
        k -= 1;
        printTable(puzzle);
    }

    if (k) {
        tel = (rec *) malloc(k * sizeof(rec));
        findValueRec(puzzle, tel);
        for (i = k-1; i >= 0; i--){
            printf("Cell %c %c value %c assumption\n\n", tel[i].ln + 1, tel[i].col + 1, tel[i].val);
            puzzle[tel[i].ln - 48][tel[i].col - 48] = tel[i].val;
            printTable(puzzle);
        }
        free(tel);
    }
    free(puzzle);
    free(puzzleStruct);
    return 0;
}

void printTable(char **puzzle) {
    int i, j;
    printf("    ");
    for (i = 1; i <= 9;i++)
        printf("%d   ",i);
    putchar('\n');
    for (i = 0; i < 9; i++) {
        if (i==0 || i==3 || i==6)
            printf("  +-----------+-----------+-----------+\n");
        else
            printf("  |           |           |           |\n");
        printf("%d | ", i+1);
        for (j = 0; j < 9; j++) {
            printf("%c ", puzzle[i][j] != '0' ? puzzle[i][j] : 32);
            if (j==2 || j==5 || j==8)
                printf("| ");
            else
                printf("  ");
        }
        putchar('\n');      
    }
    printf("  +-----------+-----------+-----------+\n\n");
    system("pause");
}

int findPosition(char **puzzle, int *i, int *j, int skip) {
    int s = 0;
    for (*i = 0; *i < 9; (*i)++)
        for (*j = 0; *j < 9; (*j)++)
            if (puzzle[*i][*j] == '0' && s++ == skip)
                return 1;
    return 0;
}

int findValueRec(char **puzzle, rec *tel) {
    int i, row, col, found = 0, flag = 0;
    static int p = 0;
    if (findPosition(puzzle, &row, &col, 0)) {
        for (i = 1; i < 10 && !flag; i++) {
            found = 0;
            if (validNum(puzzle, i, row, col)) {
                puzzle[row][col] = i + 48;
                found = 1;
                flag = findValueRec(puzzle, tel);
            }
        }
        if (!flag && (!found || puzzle[row][col] == '9')) {
            puzzle[row][col] = '0';
            return 0;
        }
        tel[p].ln = row + 48;
        tel[p].col = col + 48;
        tel[p++].val = puzzle[row][col];
        puzzle[row][col] = '0';
        return 1;
    }
    return 1;
}

int findValue(char **puzzle, int m) {
    int row, col, i;
    char *missingValues;
    missingValues = (char *) calloc(9,1);
    for (i = 0; i < m; i++) {
        if (findPosition(puzzle, &row, &col, i)) {
            if (missingNums(puzzle, missingValues, row, col)) {
                puzzle[row][col] = missingValues[0];
                printf("Cell %d %d value %c because is the only number matching\n\n", row + 1, col + 1, puzzle[row][col]);
                free(missingValues);
                return 1;
            }
        }
        else {
            free(missingValues);
            return 0;
        }
    }
    free(missingValues);
    return 0;
}

int missingNums(char **puzzle, char *missingValues, int row, int col) {
    int i, j, p=0, v = 0, found;
    char *values;
    values = (char *) calloc(30, 1);
    for (i = 0; i < 9; i++) {
        if (puzzle[row][i] != '0')
            values[p++] = puzzle[row][i];
        if (puzzle[i][col] != '0')
            values[p++] = puzzle[i][col];
    }
    for (i = (row < 3 ? 0 : (row < 6 ? 3 : 6)); i < (row < 3 ? 3 : (row < 6 ? 6 : 9)); i++)
        for (j = (col < 3 ? 0 : (col < 6 ? 3 : 6)); j < (col < 3 ? 3 : (col < 6 ? 6 : 9)); j++)
            if (puzzle[i][j] != '0')
                values[p++] = puzzle[i][j];
    for (i = 1; i < 10; i++) {
        found = 0;
        for (j = 0; j < 30 && !found; j++)
            if (values[j] == i + 48)
                found = 1;
        if (!found)
            missingValues[v++] = i + 48;
    }
    free(values);
    if (v == 1)   
        return 1;
    return 0;
}

int validNum(char **puzzle, int num, int row, int col) {
    int i, j;
    for (i = 0; i < 9; i++)
        if (puzzle[row][i] == num + 48 || puzzle[i][col] == num + 48)
            return 0;
    for (i = (row < 3 ? 0 : (row < 6 ? 3 : 6)); i < (row < 3 ? 3 : (row < 6 ? 6 : 9)); i++)
        for (j = (col < 3 ? 0 : (col < 6 ? 3 : 6)); j < (col < 3 ? 3 :(col < 6 ? 6 : 9)); j++)
            if (puzzle[i][j] == num + 48)
                return 0;
    return 1;
}
