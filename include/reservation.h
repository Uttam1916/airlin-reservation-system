#ifndef RESERVATION_H
#define RESERVATION_H

#include "queue.h"
#include "hash.h"
#include "graph.h"

#define MAX_FLIGHTS 200

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    int srcIdx;
    int destIdx;
    int capacity;
    int booked;
    Queue waitlist;
} Flight;

typedef struct {
    Flight flights[MAX_FLIGHTS];
    int n;
} FlightDB;

void flightdb_init(FlightDB *db);
int  flightdb_add(FlightDB *db, Graph *g, int id, const char *name, const char *src, const char *dest, int capacity);
Flight* flightdb_get(FlightDB *db, int id);
void flightdb_list(FlightDB *db, Graph *g);

int book_ticket(FlightDB *db, HashTable *ht, int flightId, PassengerID pid, const char *name, const char *phone);
int cancel_ticket(FlightDB *db, HashTable *ht, int flightId, PassengerID pid);
void flight_status(FlightDB *db, HashTable *ht, int flightId);

#endif
