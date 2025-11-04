#include "../include/hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int hash_func(HashTable *ht, PassengerID pid) {
    int h = pid % ht->size;
    return h < 0 ? h + ht->size : h;
}

HashTable *ht_create(int size) {
    HashTable *ht = malloc(sizeof(HashTable));
    ht->buckets = calloc(size, sizeof(Passenger *));
    ht->size = size;
    return ht;
}

void ht_free(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        Passenger *p = ht->buckets[i];
        while (p) {
            Passenger *tmp = p->next;
            free(p);
            p = tmp;
        }
    }
    free(ht->buckets);
    free(ht);
}

int ht_insert(HashTable *ht, PassengerID pid, const char *name, const char *phone) {
    int h = hash_func(ht, pid);
    Passenger *p = malloc(sizeof(Passenger));
    if (!p) return -1;
    p->pid = pid;
    strncpy(p->name, name, MAX_NAME_LEN - 1);
    p->name[MAX_NAME_LEN - 1] = '\0';
    strncpy(p->phone, phone, MAX_PHONE_LEN - 1);
    p->phone[MAX_PHONE_LEN - 1] = '\0';
    p->booking.seatNo = -1;
    p->booking.flightName[0] = '\0';
    p->next = ht->buckets[h];
    ht->buckets[h] = p;
    return 0;
}

Passenger *ht_find(HashTable *ht, PassengerID pid) {
    int h = hash_func(ht, pid);
    Passenger *p = ht->buckets[h];
    while (p) {
        if (p->pid == pid) return p;
        p = p->next;
    }
    return NULL;
}

int ht_remove(HashTable *ht, PassengerID pid) {
    int h = hash_func(ht, pid);
    Passenger **cur = &ht->buckets[h];
    while (*cur) {
        if ((*cur)->pid == pid) {
            Passenger *tmp = *cur;
            *cur = tmp->next;
            free(tmp);
            return 0;
        }
        cur = &(*cur)->next;
    }
    return -1;
}

int ht_update_booking(HashTable *ht, PassengerID pid, const char *flightName, int seatNo) {
    Passenger *p = ht_find(ht, pid);
    if (!p) return -1;
    strncpy(p->booking.flightName, flightName, MAX_NAME_LEN - 1);
    p->booking.flightName[MAX_NAME_LEN - 1] = '\0';
    p->booking.seatNo = seatNo;
    return 0;
}

void ht_display(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        Passenger *p = ht->buckets[i];
        if (p) printf("[%d] ", i);
        while (p) {
            printf("(%d,%s,%s,Seat:%d) ", p->pid, p->name, p->booking.flightName, p->booking.seatNo);
            p = p->next;
        }
        if (ht->buckets[i]) printf("\n");
    }
}
