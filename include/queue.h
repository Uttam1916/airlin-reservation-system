#ifndef QUEUE_H
#define QUEUE_H

#include "hash.h"

/*
 Priority Queue used for waitlist.
 Higher priority = earlier position in queue.
 Priority comes from Frequent Flyer points.
*/

typedef struct QNode {
    PassengerID pid;
    char name[MAX_NAME_LEN];
    int priority;            /* frequent flyer points */
    struct QNode *next;
} QNode;

typedef struct Queue {
    QNode *head;
    QNode *tail;
    int size;
} Queue;

void queue_init(Queue *q);
void queue_free(Queue *q);

/* Insert in descending priority order */
int queue_enqueue(Queue *q, PassengerID pid, const char *name, int priority);

/* Remove highest priority (head) */
int queue_dequeue(Queue *q, PassengerID *out_pid, char *out_name);

int queue_is_empty(Queue *q);
int queue_size(Queue *q);

#endif /* QUEUE_H */
