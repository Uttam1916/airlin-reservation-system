#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/graph.h"
#include "include/queue.h"
#include "include/hash.h"
#include "include/reservation.h"

int main() {
    Graph g;
    Flight flight;
    initGraph(&g);
    initHashTable();
    initFlight(&flight, "IndiGo 6E-101");

    addCity(&g, "Bengaluru");
    addCity(&g, "Delhi");
    addCity(&g, "Mumbai");
    addRoute(&g, "Bengaluru", "Delhi", 1740);
    addRoute(&g, "Bengaluru", "Mumbai", 980);
    addRoute(&g, "Mumbai", "Delhi", 1130);

    int choice, id;
    char name[50], src[30], dest[30];

    while (1) {
        printf("\n=== Airline Reservation System ===\n");
        printf("1. Display Routes\n2. Book Ticket\n3. Cancel Ticket\n4. Passenger Lookup\n5. Suggest Alternate Route\n6. Display Flight Status\n0. Exit\n> ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                displayRoutes(&g);
                break;
            case 2:
                printf("Enter Passenger ID: "); scanf("%d", &id);
                printf("Enter Name: "); scanf("%s", name);
                bookTicket(&flight, id, name);
                break;
            case 3:
                printf("Enter Passenger ID to cancel: "); scanf("%d", &id);
                cancelTicket(&flight, id);
                break;
            case 4:
                printf("Enter Passenger ID to search: "); scanf("%d", &id);
                searchPassenger(id);
                break;
            case 5:
                printf("Enter source city: "); scanf("%s", src);
                printf("Enter destination city: "); scanf("%s", dest);
                suggestAlternateRoutes(&g, src, dest);
                break;
            case 6:
                displayFlightStatus(&flight);
                break;
            case 0:
                printf("Exiting...\n");
                exit(0);
        }
    }
}
