#include <stdlib.h>
#include <stdio.h>
#include "linkedList.h"

int Member(int value, struct Node *head_p)
{
    struct Node *curr_p = head_p;

    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    if (curr_p == NULL || curr_p->data > value)
        return 0;
    else
        return 1;
}

int Insert(int value, struct Node **head_pp)
{
    struct Node *curr_p = *head_pp;
    struct Node *prev_p = NULL;
    struct Node *temp_p;

    while (curr_p != NULL && curr_p->data < value)
    {
        prev_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = (struct Node *) malloc(sizeof(struct Node));
        if (temp_p == NULL) {
            printf("Error: malloc failed\n");
            exit(EXIT_FAILURE);
        }
        temp_p->data = value;
        temp_p->next = curr_p;

        if (prev_p == NULL) 
            *head_pp = temp_p;
        else
            prev_p->next = temp_p;

        return 1;
    }
    else
        return 0;
} 

int Delete(int value, struct Node **head_pp)
{
    struct Node *curr_p = *head_pp;
    struct Node *prev_p = NULL;

    while (curr_p != NULL && curr_p->data < value)
    {
        prev_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value)
    {
        if (prev_p == NULL)
        {
            *head_pp = curr_p->next;
            free(curr_p);
        }
        else
        {
            prev_p->next = curr_p->next;
            free(curr_p);
        }

        return 1;
    }
    else
        return 0;
} 

struct Node *CopyList(struct Node *head) {
    if (head == NULL) {
        return NULL;
    }

    struct Node *new_head = malloc(sizeof(struct Node));
    new_head->data = head->data;
    new_head->next = NULL;

    struct Node *current_new_node = new_head;
    struct Node *current_orig_node = head->next;

    while (current_orig_node != NULL) {
        struct Node *new_node = malloc(sizeof(struct Node));
        new_node->data = current_orig_node->data;
        new_node->next = NULL;
        
        current_new_node->next = new_node;
        current_new_node = new_node;
        current_orig_node = current_orig_node->next;
    }

    return new_head;
} 

void FreeList(struct Node *head) {
    struct Node *current_node = head;
    while (current_node != NULL) {
        struct Node *next_node = current_node->next;
        free(current_node);
        current_node = next_node;
    }
} 