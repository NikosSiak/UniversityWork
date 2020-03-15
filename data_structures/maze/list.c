#include "list.h"
#include <stdio.h>
#include <stdlib.h>


void append(list **head, int ln, int col) {
    list *tmp, *cur;
    cur = (*head);
    tmp = (list *) malloc(sizeof(list));
    tmp->ln = ln;
    tmp->col = col;
    tmp->next = NULL;
    if ((*head) == NULL)
        (*head) = tmp;
    else {
        while (cur->next != NULL)
            cur = cur->next;
        cur->next = tmp;
    }
}


void printList(list *head) {
    list *cur;
    cur = head;
    if (cur->next != NULL)
        printList(cur->next);
    printf("(%d,%d) ", cur->ln, cur->col);
}
