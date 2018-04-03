#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct queue {
    int size;
    struct queue_node *head;
} queue;

typedef struct queue_node {
    char data[256];
    struct queue_node* before;
    struct queue_node* after;
} queue_node;


queue* create_queue() {
    queue* q;
    q = malloc(sizeof(queue));
    q->size = 0;
    q->head = NULL;
    return q;
}

int print_queue(queue* q) {
    if (q == NULL || q->size == 0) {
        printf("empty fifo\n");
        return 0;
    }
    queue_node* iter;
    iter = q->head;
    printf("%s,", iter->data);
    while (iter->after != NULL) {
        iter = iter->after;
        printf("%s,", iter->data);
    }
    printf("\n");
    return 1;
}

int push_queue(queue* q, char* str) {
    if (q == NULL) {
        printf("fifo doesnt exist\n");
        return 0;
    }
    queue_node* node;
    node = malloc(sizeof(queue_node));
    node->after = NULL;
    node->before = NULL;
    strcpy(node->data, str);

    if (q->size == 0) {
        q->head = node;
        q->size++;
        return 1;
    }

    queue_node* iter;
    iter = q->head;
    while (iter->after != NULL) {
        iter = iter->after;
    }
    iter->after = node;
    node->before = iter;
    q->size++;
    return 1;
}

char* pop_queue(queue* q) {
    if (q == NULL || q->size < 1) {
        //printf("fifo doesnt exist\n");
        return NULL;
    }
    if (q->size == 1) {
        char* str = malloc(256 * sizeof(char));
        if (str == NULL) {
            printf("could not malloc pop\n");
            exit(1);
        }
        strcpy(str, q->head->data);
        //free(q->head);
        q->head = NULL;
        q->size--;
        return str;
    }
    char* str = malloc(256 * sizeof(char));
    if (str == NULL) {
        printf("could not malloc pop\n");
        exit(1);
    }
    strcpy(str, q->head->data);
    q->head = q->head->after;
    //free(q->head->before);
    q->head->before = NULL;
    q->size--;
    return str;
}
/*
char* peek_queue(queue* q) {
    if (q == NULL || q->size < 1) {
        printf("fifo doesnt exist\n");
        return "";
    }
    char* str = malloc(256 * sizeof(char));
    if (str == NULL) {
        printf("could not malloc pop\n");
        exit(1);
    }
    strcpy(str, *q->head->data);
    return str;
}
*/