// linkedList.h

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct Node {
    int data;
    struct Node *next;
};

int Member(int value, struct Node *head_p);
int Insert(int value, struct Node **head_pp);
int Delete(int value, struct Node **head_pp);
struct Node *CopyList(struct Node *head_p);
void FreeList(struct Node *head_pp);

#endif