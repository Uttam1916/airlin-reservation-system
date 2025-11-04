#include "../include/queue.h"
#include <stdlib.h>
#include <string.h>

void queue_init(Queue *q) {
    q->head = q->tail = NULL;
    q->size = 0;
}

void queue_free(Queue *q) {
    QNode *cur = q->head;
    while (cur) {
        QNode *tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    q->head = q->tail = NULL;
    q->size = 0;
}

int queue_enqueue(Queue *q, PassengerID pid, const char *name) {
    QNode *n = malloc(sizeof(QNode));
    if (!n) return -1;
    n->pid = pid;
    strncpy(n->name, name, MAX_NAME_LEN - 1);
    n->name[MAX_NAME_LEN - 1] = '\0';
    n->next = NULL;
    if (!q->tail) q->head = q->tail = n;
    else { q->tail->next = n; q->tail = n; }
    q->size++;
    return 0;
}

int queue_dequeue(Queue *q, PassengerID *out_pid, char *out_name) {
    if (!q->head) return -1;
    QNode *n = q->head;
    if (out_pid) *out_pid = n->pid;
    if (out_name) strncpy(out_name, n->name, MAX_NAME_LEN);
    q->head = n->next;
    if (!q->head) q->tail = NULL;
    free(n);
    q->size--;
    return 0;
}

int queue_is_empty(Queue *q) { return q->head == NULL; }
int queue_size(Queue *q) { return q->size; }
