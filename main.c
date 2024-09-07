#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next; 
} Node;

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
        return 1; //updated

    } else {
        return 0; //Value already in the list
    }
}


// Delete a node
int Delete(int value, Node** head_pp) {
    Node* curr_p = *head_pp;
    Node* pred_p = NULL;

    // Find the node to be deleted
    while (curr_p != NULL && curr_p->data != value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // If the value is not found
    if (curr_p == NULL) {
        return 0;
    }

    // If the node to be deleted is the head node
    if (pred_p == NULL) {
        *head_pp = curr_p->next;
    } else {
        pred_p->next = curr_p->next; //Bypass the link
    }

    free(curr_p);  // Free the memory of the deleted node
    return 1; 
}




int main() {
    // Allocate memory for nodes
    Node* node1 = malloc(sizeof(Node));
    node1->data = 5;
    node1->next = NULL;

    Node* node2 = malloc(sizeof(Node));
    node2->data = 10;
    node2->next = NULL;
    node1->next = node2;

    Node* node3 = malloc(sizeof(Node));
    node3->data = 12;
    node3->next = NULL;
    node2->next = node3;

    //insert a value
    int isInsert = Insert(14,&node1);
    printf("Insert %d : %s \n", 14 ,isInsert ? "Yes" : "No");

    // Check membership
    int isMember1 = Member(10, node1);
    printf("%s \n", isMember1 ? "Yes" : "No");


    //Delete a node
    int isDelete = Delete(10,&node1);
    printf("Delete %d : %s \n", 10 ,isDelete ? "Yes" : "No");

    // Check membership
    int isMember2 = Member(10, node1);
    printf("%s \n", isMember2 ? "Yes" : "No");

    // Free allocated memory
    free(node3);
    free(node2);
    free(node1);

    return 0;
}
