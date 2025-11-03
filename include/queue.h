#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE 50

typedef struct {
    char names[MAX_QUEUE][50];
    int front, rear;
} Queue;

void initQueue(Queue *q);
int isFull(Queue *q);
int isEmpty(Queue *q);
void enqueue(Queue *q, const char *name);
void dequeue(Queue *q);
void displayQueue(Queue *q);

#endif
