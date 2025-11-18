#include "../include/hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int hash_func(HashTable *ht, PassengerID pid) {
    int h = pid % ht->size;
    return (h < 0) ? h + ht->size : h;
}

HashTable *ht_create(int size) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (!ht) return NULL;

    ht->buckets = calloc(size, sizeof(Passenger *));
    ht->size = size;

    return ht;
}

void ht_free(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        Passenger *p = ht->buckets[i];
        while (p) {
            Passenger *tmp = p;
            p = p->next;
            free(tmp);
        }
    }
    free(ht->buckets);
    free(ht);
}

/* Insert a passenger if not already present */
int ht_insert(HashTable *ht, PassengerID pid, const char *name, const char *phone) {
    int h = hash_func(ht, pid);

    /* Prevent duplicate PID */
    Passenger *chk = ht->buckets[h];
    while (chk) {
        if (chk->pid == pid) return 0;
        chk = chk->next;
    }

    Passenger *p = malloc(sizeof(Passenger));
    if (!p) return -1;

    p->pid = pid;
    strncpy(p->name, name, MAX_NAME_LEN - 1);
    p->name[MAX_NAME_LEN - 1] = '\0';

    strncpy(p->phone, phone, MAX_PHONE_LEN - 1);
    p->phone[MAX_PHONE_LEN - 1] = '\0';

    p->booking.flightId = -1;
    p->booking.seatNo = -1;
    p->booking.flightName[0] = '\0';
    p->booking.depart_time[0] = '\0';

    p->points_balance = 0;

    p->next = ht->buckets[h];
    ht->buckets[h] = p;

    return 0;
}

Passenger *ht_find(HashTable *ht, PassengerID pid) {
    int h = hash_func(ht, pid);

    Passenger *p = ht->buckets[h];
    while (p) {
        if (p->pid == pid)
            return p;
        p = p->next;
    }
    return NULL;
}

int ht_remove(HashTable *ht, PassengerID pid) {
    int h = hash_func(ht, pid);
    Passenger **pp = &ht->buckets[h];

    while (*pp) {
        if ((*pp)->pid == pid) {
            Passenger *tmp = *pp;
            *pp = tmp->next;
            free(tmp);
            return 0;
        }
        pp = &((*pp)->next);
    }
    return -1;
}

int ht_update_booking(HashTable *ht, PassengerID pid,
                      int flightId, const char *flightName,
                      int seatNo, const char *depart_time)
{
    Passenger *p = ht_find(ht, pid);
    if (!p) return -1;

    p->booking.flightId = flightId;
    p->booking.seatNo = seatNo;

    if (flightName)
        strncpy(p->booking.flightName, flightName, MAX_NAME_LEN - 1);

    if (depart_time)
        strncpy(p->booking.depart_time, depart_time, 31);

    return 0;
}

void ht_display(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        Passenger *p = ht->buckets[i];

        if (p) printf("[%d] ", i);

        while (p) {
            printf("(%d, %s, %s, pts:%d, F:%d/%s seat:%d @%s) ",
                   p->pid, p->name, p->phone,
                   p->points_balance,
                   p->booking.flightId, p->booking.flightName,
                   p->booking.seatNo,
                   p->booking.depart_time);
            p = p->next;
        }

        if (ht->buckets[i]) printf("\n");
    }
}

/* Add frequent flyer points */
int ht_add_points(HashTable *ht, PassengerID pid, int points) {
    Passenger *p = ht_find(ht, pid);
    if (!p) return -1;

    p->points_balance += points;
    if (p->points_balance < 0)
        p->points_balance = 0;

    return 0;
}

/* Get current points */
int ht_get_points(HashTable *ht, PassengerID pid) {
    Passenger *p = ht_find(ht, pid);
    return p ? p->points_balance : -1;
}
