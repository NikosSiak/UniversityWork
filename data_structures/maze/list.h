#ifndef LIST_H
#define LIST_H

typedef struct list{
    int ln, col;
    struct list *next;
} list;

void append(list **head, int ln, int col);

void printList(list *head);

#endif // LIST_H
