#include "tree.h"
#include <stdio.h>
#include <stdlib.h>


tree *insert(tree **root, int ln, int col) {
    tree *tmp, *cur;
    cur = (*root);
    tmp = (tree *) malloc(sizeof(tree));
    tmp->ln = ln;
    tmp->col = col;
    tmp->up = tmp->down = tmp->right = tmp->left = NULL;
    if ((*root) == NULL)
        (*root) = tmp;
    else {
        if (cur->ln < ln)
            cur->down = tmp;
        else if (cur->ln > ln)
            cur->up = tmp;
        else if (cur->col > col)
            cur->left = tmp;
        else if (cur->col < col)
            cur->right = tmp;
    }
    return tmp;
}


void printTree(tree *root) {
    static int h = 0;
    int i;
    if (root == NULL) {
        h--;
        return;
    }
    for (i = 0; i < h; i++)
        printf("   |");
    printf("(%d,%d)\n", root->ln, root->col);
    h++;
    printTree(root->left);
    h++;
    printTree(root->right);
    h++;
    printTree(root->up);
    h++;
    printTree(root->down);
    h--;
}


int countKids(tree *node) {
    int c = 0;
    if (node->up != NULL)
        c++;
    if (node->down != NULL)
        c++;
    if (node->right != NULL)
        c++;
    if (node->left != NULL)
        c++;
    return c;
}
