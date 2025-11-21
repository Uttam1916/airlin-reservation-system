#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/graph.h"
#include "include/hash.h"
#include "include/reservation.h"

#define ADMIN_PASSWORD "admin123"

void user_menu(FlightDB *db, Graph *g, HashTable *ht);
void admin_menu(FlightDB *db, Graph *g, HashTable *ht);

static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void test_priority_queue(FlightDB *db, HashTable *ht, Graph *g) {
    printf("\n===== PRIORITY QUEUE TEST =====\n");

    int fid = 1;  
    Flight *f = flightdb_get(db, fid);

    f->capacity = 2;
    f->booked   = 0;
    queue_free(&f->waitlist);
    queue_init(&f->waitlist);

    printf("Flight %d capacity forced to 2 seats\n", fid);

    book_ticket(db, ht, g, fid, 101, "Alice", "999", 30, 0); 
    book_ticket(db, ht, g, fid, 102, "Bob", "999", 30, 0);
    printf("\n--- Adding Charlie (should get HIGH price → more points) ---\n");
    book_ticket(db, ht, g, fid, 103, "Charlie", "999", 30, 0);

    printf("\n--- Adding Diana (same) ---\n");
    book_ticket(db, ht, g, fid, 104, "Diana", "999", 30, 0);

    printf("\n--- Now check waitlist order ---\n");
    QNode *q = f->waitlist.head;
    while (q) {
        printf("Waitlist: PID=%d name=%s points=%d\n",
               q->pid, q->name, ht_get_points(ht, q->pid));
        q = q->next;
    }

    printf("\n--- Cancel seat to see who is promoted ---\n");
    cancel_ticket(db, ht, fid, 101);

    printf("\n--- Check bookings after promotion ---\n");
    Passenger *p103 = ht_find(ht, 103);
    Passenger *p104 = ht_find(ht, 104);

    printf("Charlie = seat %d\n", p103->booking.seatNo);
    printf("Diana   = seat %d\n", p104->booking.seatNo);

    printf("\n===== END PRIORITY QUEUE TEST =====\n");
}


int main(void) {
    Graph g;
    initGraph(&g);

    addCity(&g, "Delhi");
    addCity(&g, "Mumbai");
    addCity(&g, "Dubai");
    addCity(&g, "London");
    addCity(&g, "NewYork");
    addCity(&g, "Toronto");
    addCity(&g, "Paris");
    addCity(&g, "Rome");
    addCity(&g, "Berlin");
    addCity(&g, "Warsaw");
    addCity(&g, "Moscow");
    addCity(&g, "Beijing");
    addCity(&g, "Tokyo");
    addCity(&g, "Sydney");
    addCity(&g, "Singapore");

    addRoute(&g, "Delhi", "Mumbai", 1400);
    addRoute(&g, "Mumbai", "Dubai", 1900);
    addRoute(&g, "Dubai", "London", 5500);
    addRoute(&g, "London", "NewYork", 5567);
    addRoute(&g, "NewYork", "Toronto", 800);
    addRoute(&g, "Toronto", "Paris", 6000);
    addRoute(&g, "Paris", "Rome", 1100);
    addRoute(&g, "Rome", "Berlin", 1180);
    addRoute(&g, "Berlin", "Warsaw", 570);
    addRoute(&g, "Warsaw", "Moscow", 1150);
    addRoute(&g, "Moscow", "Beijing", 5800);
    addRoute(&g, "Beijing", "Tokyo", 2100);
    addRoute(&g, "Tokyo", "Sydney", 7800);
    addRoute(&g, "Sydney", "Singapore", 6300);
    addRoute(&g, "Singapore", "Delhi", 4150);

    HashTable *ht = ht_create(103);
    if (!ht) {
        fprintf(stderr, "Failed to create passenger table\n");
        return 1;
    }

    FlightDB db;
    flightdb_init(&db, &g);

    test_priority_queue(&db, ht, &g); 
    user_menu(&db, &g, ht);

    ht_free(ht);
    return 0;
}

void user_menu(FlightDB *db, Graph *g, HashTable *ht) {
    for (;;) {
        printf(
            "\n================ USER MENU ================\n"
            "1. List flights\n"
            "2. List routes (with fares)\n"
            "3. Book a flight\n"
            "4. Cancel ticket\n"
            "5. View my bookings\n"
            "6. Login as admin\n"
            "0. Exit\n"
            "===========================================\n"
            "> "
        );

        int ch;
        if (scanf("%d", &ch) != 1) { flush_stdin(); continue; }

        if (ch == 0) {
            printf("Goodbye.\n");
            return;
        }

        if (ch == 1) {
            flightdb_list(db, g);
        }

        else if (ch == 2) {
            char src[MAX_NAME_LEN], dest[MAX_NAME_LEN];
            printf("Enter source city: "); scanf("%s", src);
            printf("Enter destination city: "); scanf("%s", dest);
            list_routes_with_fares(g, db, src, dest);
        }

        else if (ch == 3) {
            int fid, pid, age, is_student;
            char name[MAX_NAME_LEN], phone[MAX_PHONE_LEN];
            printf("Flight ID: "); if (scanf("%d", &fid) != 1) { flush_stdin(); continue; }
            printf("Passenger ID: "); if (scanf("%d", &pid) != 1) { flush_stdin(); continue; }
            printf("Passenger name: "); scanf("%s", name);
            printf("Phone: "); scanf("%s", phone);
            printf("Age: "); if (scanf("%d", &age) != 1) { flush_stdin(); continue; }
            printf("Student? (1 = yes / 0 = no): "); if (scanf("%d", &is_student) != 1) { flush_stdin(); continue; }
            book_ticket(db, ht, g, fid, pid, name, phone, age, is_student);
        }

        else if (ch == 4) {
            int fid, pid;
            printf("Flight ID: "); if (scanf("%d", &fid) != 1) { flush_stdin(); continue; }
            printf("Passenger ID: "); if (scanf("%d", &pid) != 1) { flush_stdin(); continue; }
            cancel_ticket(db, ht, fid, pid);
        }

        else if (ch == 5) {
            PassengerID pid;
            printf("Passenger ID: "); if (scanf("%d", &pid) != 1) { flush_stdin(); continue; }
            Passenger *p = ht_find(ht, pid);
            if (!p) printf("Passenger not found.\n");
            else {
                printf("ID:%d  %s  %s\nPoints: %d\nBooking: Flight %d seat %d depart:%s\n",
                       p->pid, p->name, p->phone, p->points_balance,
                       p->booking.flightId, p->booking.seatNo,
                       p->booking.depart_time);
            }
        }

        else if (ch == 6) {
            char pass[64];
            printf("Admin Password: "); scanf("%s", pass);
            if (strcmp(pass, ADMIN_PASSWORD) == 0) {
                printf("Admin login successful.\n");
                admin_menu(db, g, ht);
            } else {
                printf("Incorrect password.\n");
            }
        }

        else {
            printf("Invalid choice\n");
        }
    }
}

void admin_menu(FlightDB *db, Graph *g, HashTable *ht) {
    for (;;) {
        printf(
            "\n================ ADMIN MENU ================\n"
            "1. Add flight\n"
            "2. Remove flight\n"
            "3. List flights\n"
            "4. Lookup passenger\n"
            "5. Add city\n"
            "0. Logout admin\n"
            "============================================\n"
            "> "
        );

        int ch;
        if (scanf("%d", &ch) != 1) { flush_stdin(); continue; }

        if (ch == 0) {
            printf("Logging out admin.\n");
            return;
        }

        if (ch == 1) {
            int id, seats;
            char name[MAX_NAME_LEN], src[MAX_NAME_LEN], dest[MAX_NAME_LEN], depart[64];
            printf("Flight ID: "); if (scanf("%d", &id) != 1) { flush_stdin(); continue; }
            printf("Flight Name: "); scanf("%s", name);
            printf("Source: "); scanf("%s", src);
            printf("Destination: "); scanf("%s", dest);
            printf("Seats: "); if (scanf("%d", &seats) != 1) { flush_stdin(); continue; }
            flush_stdin();
            printf("Departure (YYYY-MM-DD HH:MM): "); if (!fgets(depart, sizeof(depart), stdin)) depart[0] = '\0';
            depart[strcspn(depart, "\n")] = '\0';
            int r = flightdb_add(db, g, id, name, src, dest, seats, depart);
            if (r == 0) printf("Flight added.\n");
            else if (r == -2) printf("Invalid source/destination (city not found).\n");
            else printf("Failed to add flight (DB full).\n");
        }

        else if (ch == 2) {
            int id; printf("Flight ID to remove: "); if (scanf("%d", &id) != 1) { flush_stdin(); continue; }
            if (flightdb_remove(db, id) == 0) printf("Removed.\n"); else printf("Not found.\n");
        }

        else if (ch == 3) {
            flightdb_list(db, g);
        }

        else if (ch == 4) {
            PassengerID pid; printf("Passenger ID: "); if (scanf("%d", &pid) != 1) { flush_stdin(); continue; }
            Passenger *p = ht_find(ht, pid);
            if (!p) printf("Not found.\n");
            else {
                printf("ID:%d  %s  %s\nPoints:%d\nBooking: F:%d seat:%d depart:%s\n",
                       p->pid, p->name, p->phone,
                       p->points_balance,
                       p->booking.flightId,
                       p->booking.seatNo,
                       p->booking.depart_time);
            }
        }

        else if (ch == 5) {
            char city[MAX_NAME_LEN];
            printf("City name to add: "); 
            if (scanf("%s", city) != 1) { flush_stdin(); continue; }
            int idx = addCity(g, city);
            if (idx == -1) printf("City list full or failed to add.\n");
            else printf("Added city '%s' at index %d\n", city, idx);
        }

        else {
            printf("Invalid choice.\n");
        }
    }
}
