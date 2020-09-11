/*
Alex Giacobbi
CPSC 346
Project 1 - Programming C
*/

#include <stdio.h>

struct node {
    int data;
    struct node* next; 
};


/*
Creates a node and appends it to the end of the list
Params:
    head a reference to the head of the list
    data value to be stored in the new node
*/
void append(struct node** head, int data) {
    struct node* newNode = (struct node*) malloc(sizeof(struct node));
    newNode->next = NULL;
    newNode->data = data;

    if (*head == NULL) {
        *head = newNode;
    } else {
        struct node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }   
}


/*
Merges two sorted lists and returns a pointer to the merged list
Params:
    head1 pointer to a list
    head2 pointer to a list
Return: pointer to the merged list
*/
struct node* merge(struct node* head1, struct node* head2) {
    struct node* mergedHead = NULL;

    while (head1 != NULL && head2 != NULL) {
        if (head1->data < head2->data) {
            append(&mergedHead, head1->data);
            head1 = head1->next;
        } else {
            append(&mergedHead, head2->data);
            head2 = head2->next;
        }
    }

    while (head1 != NULL) {
        append(&mergedHead, head1->data);
        head1 = head1->next;
    }

    while (head2 != NULL) {
        append(&mergedHead, head2->data);
        head2 = head2->next;
    }
    
    return mergedHead;
}


/*
Splits a linked list into two lists
Params:
    head a pointer to the head of the list
    rest a reference for pointer to the rest of the list to be used in calling
         function
    size size of the list to be split
*/
void split(struct node* head, struct node** rest, int size) {
    for (int i = 0; i < (size / 2) - 1; i++) {
        head = head->next;
    }
    *rest = head->next;
    head->next = NULL;
}


/*
Sorts a linked list using mergesort strategy
Params:
    head a pointer to the head of the list to be sorted
    size the size of the list to be sorted
Return: a pointer to the sorted list
*/
struct node* mergesort(struct node* head, int size) {
    struct node* left = head;
    struct node* right = NULL;

    if (size == 0 || size == 1) {
        return head;
    }

    split(head, &right, size);
    left = mergesort(left, size / 2);
    right = mergesort(right, size - (size / 2));

    return merge(left, right);
}


/*
Displays the contents of the list
Params:
    head a pointer to the head of the list
*/
void displayList(struct node* head) {
    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }
    printf("\n");
}


/*
Tests mergesort on an empty list
*/
void testEmptyList() {
    struct node* list = NULL;
    struct node* sortedList = mergesort(list, 0);

    printf("Test 1 -----------------------\n");
    printf("Expected: \n");
    printf("Actual  : ");
    displayList(sortedList);
    printf("------------------------------\n\n");
}


/*
Tests mergesort on a list with length 1
*/
void testOneElement() {
    struct node* list = NULL;
    append(&list, 1);
    struct node* sortedList = mergesort(list, 0);

    printf("Test 2 -----------------------\n");
    printf("Expected: 1\n");
    printf("Actual  : ");
    displayList(sortedList);
    printf("------------------------------\n\n");
}


/*
Tests mergesort on a list with length 8
*/
void testMultipleElements() {
    struct node* list = NULL;
    append(&list, 10);
    append(&list, 3);
    append(&list, 10);
    append(&list, 4);
    append(&list, 6);
    append(&list, 1);
    append(&list, 9);
    append(&list, 2);
    struct node* sortedList = mergesort(list, 8);

    printf("Test 3 -----------------------\n");
    printf("Expected: 1 2 3 4 6 9 10 10\n");
    printf("Actual  : ");
    displayList(sortedList);
    printf("------------------------------\n\n");
}


/*
Driver for testing
*/
int main() {
    testEmptyList();
    testOneElement();
    testMultipleElements();
}
