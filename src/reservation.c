#include "../include/reservation.h"
#include <stdio.h>
#include <string.h>

void flight_init(Flight *f, const char *name) {
    strncpy(f->flightName, name, MAX_NAME_LEN - 1);
    f->availableSeats = MAX_SEATS_PER_FLIGHT;
    queue_init(&f->waitingList);
}

void book_ticket(Flight *f, HashTable *ht, PassengerID pid, const char *name, const char *phone) {
    Passenger *p = ht_find(ht, pid);
    if (!p) ht_insert(ht, pid, name, phone);
    if (f->availableSeats > 0) {
        f->availableSeats--;
        ht_update_booking(ht, pid, f->flightName, MAX_SEATS_PER_FLIGHT - f->availableSeats);
        printf("Booked %s (ID %d)\n", name, pid);
    } else {
        queue_enqueue(&f->waitingList, pid, name);
        printf("Waitlisted %s\n", name);
    }
}

void cancel_ticket(Flight *f, HashTable *ht, PassengerID pid) {
    Passenger *p = ht_find(ht, pid);
    if (!p) {
        printf("Passenger not found\n");
        return;
    }
    ht_remove(ht, pid);
    f->availableSeats++;
    if (!queue_is_empty(&f->waitingList)) {
        PassengerID next;
        char n[MAX_NAME_LEN];
        queue_dequeue(&f->waitingList, &next, n);
        ht_insert(ht, next, n, "N/A");
        f->availableSeats--;
        ht_update_booking(ht, next, f->flightName, MAX_SEATS_PER_FLIGHT - f->availableSeats);
        printf("%s moved from waitlist\n", n);
    }
}

void display_flight(Flight *f, HashTable *ht) {
    printf("\nFlight: %s\nAvailable Seats: %d\n", f->flightName, f->availableSeats);
    printf("Passengers:\n");
    ht_display(ht);
    if (queue_size(&f->waitingList) > 0) {
        printf("Waiting list size: %d\n", queue_size(&f->waitingList));
    }
}
