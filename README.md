# Linked List Operations with Synchronization

This project implements concurrent operations on a linked list using mutex and read-write lock synchronization methods. It allows you to test the performance of these synchronization techniques by measuring the elapsed time for various operations.

## Requirements

- GCC or any C compiler
- `pthread` library for threading

## File Structure

- `linkedList.c`: Contains the implementation of the linked list and its operations.
- `main.c`: Contains the main logic to run the program and handle input/output.

## Compilation

To compile the code, navigate to the project directory and run the following command:

```bash
gcc -o linked_list main.c linkedList.c -lpthread -lm
```

## Commands 
```bash
Command1 (Get elapsed time) :  ./executable <thread count> <mMember> <mInsert> <mDelete>
```
```bash
Command2 (Generate csv file) :  ./executable -gen-csv <samples> <thread count> <mMember> <mInsert> <mDelete>
```