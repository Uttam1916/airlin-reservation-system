#ifndef HASH_H
#define HASH_H

#include "utils.h"

typedef struct BookingInfo {
    int flightId;
    char flightName[MAX_NAME_LEN];
    int seatNo;
    char depart_time[32];  
} BookingInfo;

typedef struct Passenger {
    PassengerID pid;
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    BookingInfo booking;
    int points_balance;     
    struct Passenger *next;
} Passenger;

typedef struct HashTable {
    Passenger **buckets;
    int size;
} HashTable;

HashTable *ht_create(int size);
void ht_free(HashTable *ht);

int ht_insert(HashTable *ht, PassengerID pid, const char *name, const char *phone);
Passenger *ht_find(HashTable *ht, PassengerID pid);
int ht_remove(HashTable *ht, PassengerID pid);

int ht_update_booking(HashTable *ht, PassengerID pid,
                      int flightId, const char *flightName,
                      int seatNo, const char *depart_time);

void ht_display(HashTable *ht);

int ht_add_points(HashTable *ht, PassengerID pid, int points);
int ht_get_points(HashTable *ht, PassengerID pid);

#endif 
