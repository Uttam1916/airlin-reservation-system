#ifndef RESERVATION_H
#define RESERVATION_H

#include "queue.h"
#include "hash.h"
#include "graph.h"

#define MAX_SEATS_PER_FLIGHT 5

typedef struct {
    char flightName[MAX_NAME_LEN];
    int availableSeats;
    Queue waitingList;
} Flight;

void flight_init(Flight *f, const char *name);
void book_ticket(Flight *f, HashTable *ht, PassengerID pid, const char *name, const char *phone);
void cancel_ticket(Flight *f, HashTable *ht, PassengerID pid);
void display_flight(Flight *f, HashTable *ht);

#endif
