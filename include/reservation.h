#ifndef RESERVATION_H
#define RESERVATION_H

#include "queue.h"
#include "hash.h"

#define MAX_SEATS 5

typedef struct {
    char flightName[30];
    int availableSeats;
    Queue waitingList;
} Flight;

void initFlight(Flight *f, const char *name);
void bookTicket(Flight *f, int id, const char *name);
void cancelTicket(Flight *f, int id);
void displayFlightStatus(Flight *f);

#endif
