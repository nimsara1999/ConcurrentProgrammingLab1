#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "linkedList.h"

#define INSERT 0
#define DELETE 1
#define MEMBER 2

#define MAX_VALUE 65535 

// Parameters for list operations
int n = 1000, m = 10000;
int thread_count;
float mMember, mInsert, mDelete;

pthread_mutex_t mutex;
pthread_rwlock_t rwlock;



// Function to populate the list with random values
void InitList(int n, struct Node **head) {
    int value;
    for (int i = 0; i < n; i++) {
        do {
            value = rand() % (MAX_VALUE + 1);
        } while (!Insert(value, head)); // Retry if value is already in the list
    }
}


// Function to print the linked list
void DisplayList(struct Node *head) {
    struct Node *current = head;
    struct Node *previous = NULL;

    while (current != NULL) {
        if (previous != NULL) {
            printf("(%d)(%p) [gap: %ld] -> ", current->data, (void *)current, (char *)current - (char *)previous);
        } else {
            printf("(%d)(%p) -> ", current->data, (void *)current);
        }
        previous = current;
        current = current->next;
    }
    printf("NULL\n");
}


// Thread function for mutex-based synchronization
void *MutexOps(void *args) {
    struct Node *head = (struct Node *)args;
    int ops_per_thread = m / thread_count;
    int local_member_ops = ops_per_thread * mMember;
    int local_insert_ops = ops_per_thread * mInsert;
    int local_delete_ops = ops_per_thread * mDelete;
    int value;

    while (local_member_ops > 0 || local_insert_ops > 0 || local_delete_ops > 0) {
        float operation = rand() % 3;
        value = rand() % MAX_VALUE;

        if (operation == MEMBER && local_member_ops > 0) {
            pthread_mutex_lock(&mutex);
            Member(value, head);
            pthread_mutex_unlock(&mutex);
            local_member_ops--;
        } else if (operation == INSERT && local_insert_ops > 0) {
            pthread_mutex_lock(&mutex);
            Insert(value, &head);
            pthread_mutex_unlock(&mutex);
            local_insert_ops--;
        } else if (operation == DELETE && local_delete_ops > 0) {
            pthread_mutex_lock(&mutex);
            Delete(value, &head);
            pthread_mutex_unlock(&mutex);
            local_delete_ops--;
        }
    }

    return NULL;
}

// Thread function for rwlock-based synchronization
void *RWLockOps(void *args) {
    struct Node *head = (struct Node *)args;
    int ops_per_thread = m / thread_count;
    int local_member_ops = ops_per_thread * mMember;
    int local_insert_ops = ops_per_thread * mInsert;
    int local_delete_ops = ops_per_thread * mDelete;
    int value;

    while (local_member_ops > 0 || local_insert_ops > 0 || local_delete_ops > 0) {
        float operation = rand() % 3;
        value = rand() % MAX_VALUE;

        if (operation == MEMBER && local_member_ops > 0) {
            pthread_rwlock_rdlock(&rwlock);
            Member(value, head);
            pthread_rwlock_unlock(&rwlock);
            local_member_ops--;
        } else if (operation == INSERT && local_insert_ops > 0) {
            pthread_rwlock_wrlock(&rwlock);
            Insert(value, &head);
            pthread_rwlock_unlock(&rwlock);
            local_insert_ops--;
        } else if (operation == DELETE && local_delete_ops > 0) {
            pthread_rwlock_wrlock(&rwlock);
            Delete(value, &head);
            pthread_rwlock_unlock(&rwlock);
            local_delete_ops--;
        }
    }

    return NULL;
}

// Perform list operations sequentially without any synchronization
double SerialOps(struct Node *head) {
    int value;
    int total_member_ops = m * mMember;
    int total_insert_ops = m * mInsert;
    int total_delete_ops = m * mDelete;

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 0; i < total_member_ops; i++) {
        value = rand() % MAX_VALUE;
        Member(value, head);
    }

    for (int i = 0; i < total_insert_ops; i++) {
        value = rand() % MAX_VALUE;
        Insert(value, &head);
    }

    for (int i = 0; i < total_delete_ops; i++) {
        value = rand() % MAX_VALUE;
        Delete(value, &head);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Elapsed time (Serial): %.10f s\n", elapsed_time);
    return elapsed_time;
}

// Perform list operations using mutex synchronization
double MutexOpsParallel(struct Node *head) {
    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    struct timespec start_time, end_time;

    printf("Threads: %d\n", thread_count);
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, MutexOps, (void *)head);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Elapsed time (Mutex): %.10f s\n", elapsed_time);
    free(threads);
    return elapsed_time;
}

// Perform list operations using read-write lock synchronization
double RWLockOpsParallel(struct Node *head) {
    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    struct timespec start_time, end_time;

    printf("Threads: %d\n", thread_count);
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, RWLockOps, (void *)head);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Elapsed time (Rwlock): %.10f s\n", elapsed_time);
    free(threads);
    return elapsed_time;
}

// Generate a CSV file with performance results for different synchronization methods
void GenerateResultsCSV(int num_samples, int num_threads) {
    int iterations = num_samples;
    double serial_time, mutex_time, rwlock_time;

    struct Node *head = NULL;
    struct Node *list_serial, *list_mutex, *list_rwlock;

    FILE *file = fopen("output.csv", "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "threads,serial,mutex,rwlock\n");

    InitList(n, &head);


    for (int j = 0; j < iterations; j++) {
        thread_count = num_threads;
        list_mutex = CopyList(head);
        list_rwlock = CopyList(head);

        if (thread_count == 1) {
            list_serial = CopyList(head);
            serial_time = SerialOps(list_serial);
        } else {
            serial_time = 0;
        }

        mutex_time = MutexOpsParallel(list_mutex);
        rwlock_time = RWLockOpsParallel(list_rwlock);

        fprintf(file, "%d,%.10f,%.10f,%.10f\n", thread_count, serial_time, mutex_time, rwlock_time);
    }

    FreeList(head);
    FreeList(list_serial);
    FreeList(list_mutex);
    FreeList(list_rwlock);

    fclose(file);
    printf("Results saved to output.csv\n");
}

int main(int argc, char *argv[]) {
    fprintf(stderr, "Usage: %s <thread count> <mMember> <mInsert> <mDelete> | -gen-csv <samples> <mMember> <mInsert> <mDelete>\n", argv[0]);
    if (argc < 4) {
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-gen-csv") == 0) {
        if (argc != 7) {
            fprintf(stderr, "Usage: %s -gen-csv <samples> <threads> <mMember> <mInsert> <mDelete>\n", argv[0]);
            return EXIT_FAILURE;
        }
        int num_samples = atoi(argv[2]);
        int num_threads = atoi(argv[3]);
        mMember = atof(argv[4]);
        mInsert = atof(argv[5]);
        mDelete = atof(argv[6]);

        srand(time(NULL));
        GenerateResultsCSV(num_samples, num_threads);
    } else {
        thread_count = atoi(argv[1]);
        mMember = atof(argv[2]);
        mInsert = atof(argv[3]);
        mDelete = atof(argv[4]);

        srand(time(NULL));

        pthread_mutex_init(&mutex, NULL);
        pthread_rwlock_init(&rwlock, NULL);

        struct Node *head = NULL;
        InitList(n, &head);

        printf("\n");printf("\n");
        printf("Test Configurations\n");
        printf("--------------------------------------------------------------------\n");
        printf("%-40s - %21d  \n", "Number of threads", thread_count);
        printf("%-40s - %21d  \n", "n(Initial linked list size)", n);
        printf("%-40s - %21d  \n", "m(Operations to perform)", m);
        printf("%-40s - %21.3f  \n", "Percentage of mMember", mMember);
        printf("%-40s - %21.3f  \n", "Percentage of mInsert", mInsert);
        printf("%-40s - %21.3f  \n", "Percentage of mDelete", mDelete);
        printf("--------------------------------------------------------------------\n");
        printf("\n");printf("\n");

        printf("Test Results\n");
        printf("--------------------------------------------------------------------\n");
        SerialOps(head);
        printf("--------------------------------------------------------------------\n");
        MutexOpsParallel(head);
        printf("--------------------------------------------------------------------\n");
        RWLockOpsParallel(head);
        printf("--------------------------------------------------------------------\n");
        printf("\n");printf("\n");


        FreeList(head);
        pthread_mutex_destroy(&mutex);
        pthread_rwlock_destroy(&rwlock);
    }

    return EXIT_SUCCESS;
}
