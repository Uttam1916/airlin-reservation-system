#include "../include/queue.h"
#include <stdlib.h>
#include <string.h>

/* Initialize an empty priority queue */
void queue_init(Queue *q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

/* Free all nodes */
void queue_free(Queue *q) {
    QNode *cur = q->head;
    while (cur) {
        QNode *tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

/*
 Insert passenger sorted by priority (descending)
 Higher priority → earlier in queue
 If two passengers have same priority → FIFO ordering
*/
int queue_enqueue(Queue *q, PassengerID pid, const char *name, int priority) {
    QNode *node = malloc(sizeof(QNode));
    if (!node) return -1;

    node->pid = pid;
    strncpy(node->name, name, MAX_NAME_LEN - 1);
    node->name[MAX_NAME_LEN - 1] = '\0';
    node->priority = priority;
    node->next = NULL;

    /* Case 1: empty queue */
    if (!q->head) {
        q->head = q->tail = node;
    }
    else {
        /* Insert at head if highest priority */
        if (priority > q->head->priority) {
            node->next = q->head;
            q->head = node;
        }
        else {
            QNode *cur = q->head;
            QNode *prev = NULL;

            while (cur && cur->priority >= priority) {
                prev = cur;
                cur = cur->next;
            }

            prev->next = node;
            node->next = cur;

            if (!cur)
                q->tail = node;
        }
    }

    q->size++;
    return 0;
}

/* Remove head (highest priority) */
int queue_dequeue(Queue *q, PassengerID *out_pid, char *out_name) {
    if (!q->head) return -1;

    QNode *node = q->head;
    if (out_pid) *out_pid = node->pid;
    if (out_name) strncpy(out_name, node->name, MAX_NAME_LEN);

    q->head = node->next;
    if (!q->head)
        q->tail = NULL;

    free(node);
    q->size--;
    return 0;
}

int queue_is_empty(Queue *q) {
    return q->head == NULL;
}

int queue_size(Queue *q) {
    return q->size;
}
