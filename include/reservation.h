#ifndef RESERVATION_H
#define RESERVATION_H

#include "graph.h"
#include "queue.h"
#include "hash.h"

/* Flight record */
typedef struct Flight {
    int id;
    char name[MAX_NAME_LEN];

    int srcIdx;     /* index in graph cityNames[] */
    int destIdx;

    int capacity;
    int booked;

    char depart_time[32];  /* "YYYY-MM-DD HH:MM" */

    Queue waitlist;        /* priority queue based on frequent-flyer points */
} Flight;

typedef struct FlightDB {
    Flight flights[MAX_FLIGHTS];
    int n;
} FlightDB;

/* Initialize DB + preload 15 flights */
void flightdb_init(FlightDB *db, Graph *g);

/* Add/remove flights (admin) */
int flightdb_add(FlightDB *db, Graph *g, int id,
                 const char *name,
                 const char *src, const char *dest,
                 int capacity, const char *depart_time);

int flightdb_remove(FlightDB *db, int id);

Flight *flightdb_get(FlightDB *db, int id);

/* List flights (user + admin) */
void flightdb_list(FlightDB *db, Graph *g);

/* Route listing (paths sorted by number of flights) */
typedef struct RouteInfo {
    int nodes[MAX_CITIES];
    int len;
    int total_distance;
} RouteInfo;

void list_routes_with_fares(Graph *g, FlightDB *db,
                            const char *src, const char *dest);

/* Fare calculation
   prospective: if non-zero, fare computed as if booking the next seat (booked+1)
*/
int compute_fare(Graph *g, Flight *f,
                 int passenger_age, int is_student,
                 HashTable *ht,
                 int prospective);

/* Booking & cancellation */
int book_ticket(FlightDB *db, HashTable *ht, Graph *g, int flightId,
                PassengerID pid, const char *name, const char *phone,
                int passenger_age, int is_student);

int cancel_ticket(FlightDB *db, HashTable *ht, int flightId, PassengerID pid);

/* Flight status */
void flight_status(FlightDB *db, HashTable *ht, int flightId);

/* User-facing flight list */
void list_all_flights_for_user(FlightDB *db, Graph *g);

#endif /* RESERVATION_H */
