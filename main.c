#include <stdio.h>
#include <string.h>
#include "include/graph.h"
#include "include/hash.h"
#include "include/reservation.h"

int main(){
    Graph g; initGraph(&g);
    HashTable *ht=ht_create(31);
    FlightDB db; flightdb_init(&db);

    for(;;){
        int ch; printf("\n1 Add city\n2 Add route\n3 Show routes\n4 BFS route\n5 All paths\n6 Dijkstra\n7 Add flight\n8 List flights\n9 Book (by flight ID)\n10 Cancel (by flight ID)\n11 Flight status\n12 Lookup passenger\n0 Exit\n> ");
        if(scanf("%d",&ch)!=1){ while(getchar()!='\n'); continue; }
        if(ch==0) break;

        if(ch==1){
            char name[MAX_NAME_LEN]; printf("City: "); scanf("%s",name);
            int idx=addCity(&g,name); if(idx==-1) printf("Full\n"); else printf("Added %s (%d)\n",name,idx);
        } else if(ch==2){
            char a[MAX_NAME_LEN],b[MAX_NAME_LEN]; int d;
            printf("From: "); scanf("%s",a);
            printf("To: "); scanf("%s",b);
            printf("Distance: "); scanf("%d",&d);
            addRoute(&g,a,b,d); printf("OK\n");
        } else if(ch==3){
            displayRoutes(&g);
        } else if(ch==4){
            char a[MAX_NAME_LEN],b[MAX_NAME_LEN];
            printf("From: "); scanf("%s",a);
            printf("To: "); scanf("%s",b);
            suggestAlternateRoutes(&g,a,b);
        } else if(ch==5){
            char a[MAX_NAME_LEN],b[MAX_NAME_LEN];
            printf("From: "); scanf("%s",a);
            printf("To: "); scanf("%s",b);
            find_all_paths(&g,a,b);
        } else if(ch==6){
            char a[MAX_NAME_LEN],b[MAX_NAME_LEN];
            printf("From: "); scanf("%s",a);
            printf("To: "); scanf("%s",b);
            dijkstra_shortest_path(&g,a,b);
        } else if(ch==7){
            int id,cap; char n[MAX_NAME_LEN], a[MAX_NAME_LEN], b[MAX_NAME_LEN];
            printf("Flight ID: "); scanf("%d",&id);
            printf("Name: "); scanf("%s",n);
            printf("From city: "); scanf("%s",a);
            printf("To city: "); scanf("%s",b);
            printf("Capacity: "); scanf("%d",&cap);
            int r=flightdb_add(&db,&g,id,n,a,b,cap);
            if(r==0) printf("Flight added\n"); else printf("Failed\n");
        } else if(ch==8){
            flightdb_list(&db,&g);
        } else if(ch==9){
            int fid; PassengerID pid; char n[MAX_NAME_LEN], ph[MAX_PHONE_LEN];
            printf("Flight ID: "); scanf("%d",&fid);
            printf("Passenger ID: "); scanf("%d",&pid);
            printf("Name: "); scanf("%s",n);
            printf("Phone: "); scanf("%s",ph);
            book_ticket(&db,ht,fid,pid,n,ph);
        } else if(ch==10){
            int fid; PassengerID pid;
            printf("Flight ID: "); scanf("%d",&fid);
            printf("Passenger ID: "); scanf("%d",&pid);
            cancel_ticket(&db,ht,fid,pid);
        } else if(ch==11){
            int fid; printf("Flight ID: "); scanf("%d",&fid);
            flight_status(&db,ht,fid);
        } else if(ch==12){
            PassengerID id; printf("Passenger ID: "); scanf("%d",&id);
            Passenger *p=ht_find(ht,id);
            if(!p) printf("Not found\n");
            else printf("P:%d %s %s F:%d/%s Seat:%d\n",p->pid,p->name,p->phone,p->booking.flightId,p->booking.flightName,p->booking.seatNo);
        }
    }
    ht_free(ht);
    return 0;
}
