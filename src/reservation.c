#include "../include/reservation.h"
#include <stdio.h>
#include <string.h>

void flightdb_init(FlightDB *db){ db->n=0; }

int flightdb_add(FlightDB *db, Graph *g, int id, const char *name, const char *src, const char *dest, int capacity){
    if(db->n>=MAX_FLIGHTS) return -1;
    int s=getCityIndex(g,src), d=getCityIndex(g,dest);
    if(s==-1 || d==-1) return -2;
    Flight *f=&db->flights[db->n++];
    f->id=id;
    strncpy(f->name,name,MAX_NAME_LEN-1); f->name[MAX_NAME_LEN-1]='\0';
    f->srcIdx=s; f->destIdx=d;
    f->capacity=capacity; f->booked=0;
    queue_init(&f->waitlist);
    return 0;
}

Flight* flightdb_get(FlightDB *db, int id){
    for(int i=0;i<db->n;i++) if(db->flights[i].id==id) return &db->flights[i];
    return NULL;
}

void flightdb_list(FlightDB *db, Graph *g){
    printf("\nFlights:\n");
    for(int i=0;i<db->n;i++){
        Flight *f=&db->flights[i];
        printf("ID:%d %s %s->%s seats:%d/%d\n",f->id,f->name,g->cityNames[f->srcIdx],g->cityNames[f->destIdx],f->booked,f->capacity);
    }
}

int book_ticket(FlightDB *db, HashTable *ht, int flightId, PassengerID pid, const char *name, const char *phone){
    Flight *f=flightdb_get(db,flightId); if(!f) { printf("Flight not found\n"); return -1; }
    if(!ht_find(ht,pid)) ht_insert(ht,pid,name,phone);
    if(f->booked < f->capacity){
        f->booked++;
        ht_update_booking(ht,pid,f->id,f->name,f->booked);
        printf("Booked P:%d on F:%d seat:%d\n",pid,f->id,f->booked);
        return 0;
    } else {
        queue_enqueue(&f->waitlist,pid,name);
        printf("Waitlisted P:%d on F:%d\n",pid,f->id);
        return 1;
    }
}

int cancel_ticket(FlightDB *db, HashTable *ht, int flightId, PassengerID pid){
    Flight *f=flightdb_get(db,flightId); if(!f){ printf("Flight not found\n"); return -1; }
    Passenger *p=ht_find(ht,pid); if(!p || p->booking.flightId!=flightId){ printf("No booking\n"); return -1; }
    ht_update_booking(ht,pid,-1,"", -1);
    if(f->booked>0) f->booked--;
    if(!queue_is_empty(&f->waitlist)){
        PassengerID np; char nn[MAX_NAME_LEN];
        queue_dequeue(&f->waitlist,&np,nn);
        if(!ht_find(ht,np)) ht_insert(ht,np,nn,"NA");
        f->booked++;
        ht_update_booking(ht,np,f->id,f->name,f->booked);
        printf("Promoted P:%d seat:%d\n",np,f->booked);
    }
    printf("Cancelled P:%d on F:%d\n",pid,f->id);
    return 0;
}

void flight_status(FlightDB *db, HashTable *ht, int flightId){
    Flight *f=flightdb_get(db,flightId); if(!f){ printf("Flight not found\n"); return; }
    printf("Flight ID:%d %s seats:%d/%d wait:%d\n",f->id,f->name,f->booked,f->capacity,queue_size(&f->waitlist));
}
