#ifndef QUEUE_H
#define QUEUE_H

#include "hash.h"


typedef struct QNode {
    PassengerID pid;
    char name[MAX_NAME_LEN];
    int priority;            
    struct QNode *next;
} QNode;

typedef struct Queue {
    QNode *head;
    QNode *tail;
    int size;
} Queue;

void queue_init(Queue *q);
void queue_free(Queue *q);

int queue_enqueue(Queue *q, PassengerID pid, const char *name, int priority);

int queue_dequeue(Queue *q, PassengerID *out_pid, char *out_name);

int queue_is_empty(Queue *q);
int queue_size(Queue *q);

#endif 