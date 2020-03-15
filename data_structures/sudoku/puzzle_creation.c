#include <stdio.h>

int main() {
    struct rec {
        char ln;
        char col;
        char val;
    } x[25] = {
            1,1,5,
            1,2,8,
            1,3,6,
            1,5,7,
            2,4,9,
            2,6,1,
            2,7,6,
            3,4,6,
            4,3,7,
            5,1,9,
            5,3,2,
            5,5,1,
            5,7,3,
            5,9,5,
            6,3,5,
            6,5,9,
            7,2,9,
            7,5,4,
            7,9,8,
            8,3,3,
            8,4,5,
            8,8,6,
            9,5,2,
            9,7,4,
            9,8,7
        };

    FILE *fp;
    fp = fopen("puzzle1312","wb");
    fwrite(x, 1, sizeof(x), fp);
    fclose(fp);
    return 0;
}
