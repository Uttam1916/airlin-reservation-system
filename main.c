#include <stdio.h>
#include "include/graph.h"
#include "include/hash.h"
#include "include/reservation.h"

int main() {
    Graph g;
    initGraph(&g);
    addCity(&g, "Delhi");
    addCity(&g, "Mumbai");
    addCity(&g, "Bengaluru");
    addRoute(&g, "Delhi", "Mumbai", 1200);
    addRoute(&g, "Mumbai", "Bengaluru", 980);
    addRoute(&g, "Delhi", "Bengaluru", 1740);

    HashTable *ht = ht_create(31);
    Flight f;
    flight_init(&f, "IndiGo 6E-101");

    int c;
    while (1) {
        printf("\n1.Display Routes\n2.Book\n3.Cancel\n4.Show Flight\n5.Route Suggest\n0.Exit\n> ");
        scanf("%d", &c);
        if (c == 0) break;
        if (c == 1) displayRoutes(&g);
        else if (c == 2) {
            PassengerID id; char n[MAX_NAME_LEN], ph[MAX_PHONE_LEN];
            printf("ID: "); scanf("%d", &id);
            printf("Name: "); scanf("%s", n);
            printf("Phone: "); scanf("%s", ph);
            book_ticket(&f, ht, id, n, ph);
        } else if (c == 3) {
            PassengerID id; printf("ID: "); scanf("%d", &id);
            cancel_ticket(&f, ht, id);
        } else if (c == 4) display_flight(&f, ht);
        else if (c == 5) {
            char s[32], d[32];
            printf("From: "); scanf("%s", s);
            printf("To: "); scanf("%s", d);
            suggestAlternateRoutes(&g, s, d);
        }
    }
    ht_free(ht);
    return 0;
}
