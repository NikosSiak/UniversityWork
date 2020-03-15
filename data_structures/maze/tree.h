#ifndef TREE_H
#define TREE_H

typedef struct tree{
    int ln, col;
    struct tree *up, *down, *right, *left;
} tree;

tree *insert(tree **root, int ln, int col);

void printTree(tree *root);

int countKids(tree *node);

#endif // TREE_H
