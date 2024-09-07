#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


typedef struct Node {
    int data;
    struct Node *next; 
} Node;

Node* head = NULL; // Head of the linked list
// Global mutex
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

// Membership Function -> check if a value exists in the list
int Member(int value, Node* head_p) {
    Node* current_p = head_p;
    while (current_p != NULL) {
        if (current_p->data == value) {
            return 1;  
        } else if (current_p->data > value) {
            return 0;  
        }
        current_p = current_p->next;
    }
    return 0;           
}

// Insert a new node 
int Insert(int value, Node** head_pp) {
    pthread_mutex_lock(&list_mutex);

    Node* curr_p = *head_pp;  // Pointer to the current node
    Node* pred_p = NULL;      // Pointer to the previous node
    Node* temp_p;             // Pointer for the new node

    // Traverse the list to find the correct position for insertion
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = (Node*)malloc(sizeof(Node)); 
        temp_p->data = value;
        temp_p->next = curr_p;

        // If the new node is the first node or needs to be inserted at the head
        if (pred_p == NULL) {  // New first node
            *head_pp = temp_p;
        } else {
            pred_p->next = temp_p;  // Link the new node after the predecessor
        }
        pthread_mutex_unlock(&list_mutex);
        return 1; // updated

    } else {
        pthread_mutex_unlock(&list_mutex);
        return 0; // Value already in the list
    }
}

// Delete a node
int Delete(int value, Node** head_pp) {
    pthread_mutex_lock(&list_mutex);

    Node* curr_p = *head_pp;
    Node* pred_p = NULL;

    // Find the node to be deleted
    while (curr_p != NULL && curr_p->data != value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // If the value is not found
    if (curr_p == NULL) {
        pthread_mutex_unlock(&list_mutex);
        return 0;
    }

    // If the node to be deleted is the head node
    if (pred_p == NULL) {
        *head_pp = curr_p->next;
    } else {
        pred_p->next = curr_p->next; // Bypass the link
    }

    free(curr_p);  // Free the memory of the deleted node
    pthread_mutex_unlock(&list_mutex);
    return 1; 
}



// Thread function for inserting a value
void* thread_insert(void* arg) {
    int value = *(int*)arg;
    Insert(value, &head);
    return NULL;
}

// Thread function for deleting a value
void* thread_delete(void* arg) {
    int value = *(int*)arg;
    Delete(value, &head);
    return NULL;
}

int main() {
    pthread_t threads[2];
    int values[4] = {5, 10, 12, 14};

    // Create threads for insertion
    for (int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, thread_insert, &values[i]);
    }

    // Create threads for deletion
    for (int i = 2; i < 4; i++) {
        pthread_create(&threads[i], NULL, thread_delete, &values[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print the list to verify
    Node* current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");

    // Clean up
    pthread_mutex_destroy(&list_mutex);
    return 0;
}
