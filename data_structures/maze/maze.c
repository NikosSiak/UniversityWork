#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "tree.h"

tree *destinations(char **maze, tree *root, int r, int c, int ln, int col);
void exits(list **head, tree *root, int f_ln, int f_col, tree *fath, int flag, char pr);
int countExits(tree *root, int f_ln, int f_col);

int main(int argc, char *argv[]) {
    FILE *fp;
    int r, c, i, j, ln, col, f_ln, f_col, n;
    char junk, **maze;
    tree *root = NULL;
    list **e;
    
    if (argc != 2) {
        printf("Usage: %s file_name.txt\n", argv[0]);
        return 1;
    }
    
    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("Couldn't open file\n");
        return 2;
    }
    
    fscanf(fp, "%d%c%d%c", &r, &junk, &c, &junk);
    maze = (char **) malloc(r * sizeof(char *));
    for (i = 0; i < r; i++)
        maze[i] = (char *) malloc(c);
    
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++) {
            fscanf(fp, "%c", &maze[i][j]);
            if (j == c-1)
                fscanf(fp, "%c", &junk);
        }
    
    fscanf(fp, "%d%c%d%c%d%c%d", &ln, &junk, &col, &junk, &f_ln, &junk, &f_col);
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++)
            printf("%c", maze[i][j]);
        printf("\n");
    }
    
    root = destinations(maze, root, r, c, ln, col);
    printTree(root);
    printf("\n\n\n");
    n = countExits(root, f_ln, f_col);
    e = (list **) malloc(n * sizeof(list));
    for (i = 0; i < n; i++){
        e[i] = NULL;
        exits(&e[i], root, f_ln, f_col, NULL, 0, 0);
        printList(e[i]);
        putchar('\n');
    }
    fclose(fp);
    free(maze);
    free(e);
    return 0;
}

tree *destinations(char **maze, tree *root, int r, int c, int ln, int col) {
    if (maze[ln][col] == '*' || maze[ln][col] == '.')
        return root;
    else{
        root = insert(&root, ln, col);
        maze[ln][col] = '.';
        if (col-1 >= 0)
            destinations(maze, root, r, c, ln, col-1);
        if (col+1 < c)
            destinations(maze, root, r, c, ln, col+1);
        if (ln-1 >= 0)
            destinations(maze, root, r, c, ln-1, col);
        if (ln+1 < r)
            destinations(maze, root, r, c, ln+1, col);
    }
    maze[ln][col] = ' ';
    return root;
}

void exits(list **head, tree *root, int f_ln, int f_col, tree *fath, int flag, char pr) {
    static int f = 0;
    if (!flag)
        f = 0;
    if (root == NULL)
        return;
    else if ((root->ln == f_ln && root->col == f_col) || f) {
        append(head, root->ln, root->col);
        f = 1;
        if (pr == 'u')
            fath->up = NULL;
        else if (pr == 'd')
            fath->down = NULL;
        else if (pr == 'r')
            fath->right = NULL;
        else if (pr == 'l')
            fath->left = NULL;
        return;
    }
    else {
        if (!f) {
            fath = root;
            exits(head, root->up, f_ln, f_col, fath, 1, 'u');
            if (countKids(root) == 0)
                fath->up = NULL;
        }
        if (!f) {
            fath = root;
            exits(head, root->down, f_ln, f_col, fath, 1, 'd');
            if (countKids(root) == 0)
                fath->down = NULL;
        }
        if (!f) {
            fath = root;
            exits(head, root->right, f_ln, f_col, fath, 1, 'r');
            if (countKids(root) == 0)
                fath->right = NULL;
        }
        if (!f) {
            fath = root;
            exits(head, root->left, f_ln, f_col, fath, 1, 'l');
            if (countKids(root) == 0)
                fath->left = NULL;
        }
        if (f)
            append(head, root->ln, root->col);
    }
}

int countExits(tree *root, int f_ln, int f_col){
    static int c = 0;
    if (root != NULL){
        if (root->ln == f_ln && root->col == f_col)
            c++;
        countExits(root->left, f_ln, f_col);
        countExits(root->right, f_ln, f_col);
        countExits(root->up, f_ln, f_col);
        countExits(root->down, f_ln, f_col);
    }
    return c;
}
