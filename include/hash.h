#ifndef HASH_H
#define HASH_H
#include "utils.h"

typedef struct BookingRec {
    int flightId;
    char flightName[MAX_NAME_LEN];
    int seatNo;
} BookingRec;

typedef struct Passenger {
    PassengerID pid;
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    BookingRec booking;
    struct Passenger *next;
} Passenger;

typedef struct {
    Passenger **buckets;
    int size;
} HashTable;

HashTable *ht_create(int size);
void ht_free(HashTable *ht);
int ht_insert(HashTable *ht, PassengerID pid, const char *name, const char *phone);
Passenger *ht_find(HashTable *ht, PassengerID pid);
int ht_remove(HashTable *ht, PassengerID pid);
int ht_update_booking(HashTable *ht, PassengerID pid, int flightId, const char *flightName, int seatNo);
void ht_display(HashTable *ht);

#endif
