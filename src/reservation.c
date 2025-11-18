#include "../include/reservation.h"
#include "../include/hash.h"
#include "../include/queue.h"
#include "../include/graph.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* ------------------ helpers ------------------ */

static int compute_distance(Graph *g, int src, int dest) {
    int dist[MAX_CITIES], used[MAX_CITIES];
    for (int i = 0; i < g->numCities; i++) { dist[i] = INF; used[i] = 0; }
    dist[src] = 0;
    for (int it = 0; it < g->numCities; it++) {
        int u = -1, best = INF;
        for (int i = 0; i < g->numCities; i++) if (!used[i] && dist[i] < best) { best = dist[i]; u = i; }
        if (u == -1) break;
        used[u] = 1;
        for (int v = 0; v < g->numCities; v++) {
            if (g->adjMatrix[u][v] != INF && g->adjMatrix[u][v] > 0) {
                int nd = dist[u] + g->adjMatrix[u][v];
                if (nd < dist[v]) dist[v] = nd;
            }
        }
    }
    return dist[dest] >= INF ? INF : dist[dest];
}

/* extract hour from "YYYY-MM-DD HH:MM" returns -1 on parse fail */
static int parse_hour(const char *dt) {
    if (!dt || !dt[0]) return -1;
    const char *p = strchr(dt, ' ');
    if (!p) return -1;
    int hh = atoi(p + 1);
    if (hh < 0 || hh > 23) return -1;
    return hh;
}

/* peak multiplier: morning 06-09 and evening 17-21 are peak */
static double peak_multiplier(const char *dt) {
    int h = parse_hour(dt);
    if (h == -1) return 1.0;
    if ((h >= 6 && h <= 9) || (h >= 17 && h <= 21)) return 1.25;
    return 1.0;
}

/* ------------------ preload / flight db ------------------ */

/* (reuse your existing preload array or keep previous defaults) */
typedef struct Preload {
    char name[MAX_NAME_LEN];
    char src[MAX_NAME_LEN];
    char dest[MAX_NAME_LEN];
    int seats;
    char depart[32];
} Preload;

static Preload defaults[] = {
    {"AI101", "Delhi", "Mumbai", 180, "2025-11-20 09:00"},
    {"AI202", "Mumbai", "Dubai", 220, "2025-11-20 12:00"},
    {"AI303", "Dubai", "London", 250, "2025-11-20 18:00"},
    {"AI404", "London", "NewYork", 280, "2025-11-20 21:00"},
    {"AI505", "NewYork", "Toronto", 200, "2025-11-21 08:00"},
    {"AI606", "Toronto", "Paris", 230, "2025-11-21 11:00"},
    {"AI707", "Paris", "Rome", 160, "2025-11-21 15:30"},
    {"AI808", "Rome", "Berlin", 140, "2025-11-21 18:45"},
    {"AI909", "Berlin", "Warsaw", 150, "2025-11-22 06:15"},
    {"AI010", "Warsaw", "Moscow", 170, "2025-11-22 09:45"},
    {"AI111", "Moscow", "Beijing", 260, "2025-11-22 14:00"},
    {"AI222", "Beijing", "Tokyo", 210, "2025-11-22 19:30"},
    {"AI333", "Tokyo", "Sydney", 280, "2025-11-23 07:00"},
    {"AI444", "Sydney", "Singapore", 200, "2025-11-23 13:20"},
    {"AI555", "Singapore", "Delhi", 190, "2025-11-23 18:55"},
};

void flightdb_init(FlightDB *db, Graph *g) {
    db->n = 0;
    for (int i = 0; i < (int)(sizeof(defaults)/sizeof(defaults[0])); i++) {
        flightdb_add(db, g, i + 1, defaults[i].name, defaults[i].src, defaults[i].dest, defaults[i].seats, defaults[i].depart);
    }
}

int flightdb_add(FlightDB *db, Graph *g, int id,
                 const char *name, const char *src, const char *dest,
                 int capacity, const char *depart_time)
{
    if (db->n >= MAX_FLIGHTS) return -1;
    int s = getCityIndex(g, src), d = getCityIndex(g, dest);
    if (s == -1 || d == -1) return -2;
    Flight *f = &db->flights[db->n++];
    f->id = id;
    strncpy(f->name, name, MAX_NAME_LEN - 1); f->name[MAX_NAME_LEN - 1] = '\0';
    f->srcIdx = s; f->destIdx = d;
    f->capacity = capacity; f->booked = 0;
    if (depart_time) strncpy(f->depart_time, depart_time, sizeof(f->depart_time)-1);
    else f->depart_time[0] = '\0';
    f->depart_time[sizeof(f->depart_time)-1] = '\0';
    queue_init(&f->waitlist);
    return 0;
}

int flightdb_remove(FlightDB *db, int id) {
    for (int i = 0; i < db->n; i++) {
        if (db->flights[i].id == id) {
            queue_free(&db->flights[i].waitlist);
            db->flights[i] = db->flights[db->n - 1];
            db->n--;
            return 0;
        }
    }
    return -1;
}

Flight *flightdb_get(FlightDB *db, int id) {
    for (int i = 0; i < db->n; i++) if (db->flights[i].id == id) return &db->flights[i];
    return NULL;
}

void flightdb_list(FlightDB *db, Graph *g) {
    printf("\nAvailable Flights:\n");
    for (int i = 0; i < db->n; i++) {
        Flight *f = &db->flights[i];
        printf("ID:%d  %s  %s -> %s  Seats:%d/%d  Time:%s\n",
               f->id, f->name, g->cityNames[f->srcIdx], g->cityNames[f->destIdx],
               f->booked, f->capacity, f->depart_time);
    }
}

/* ------------------ compute_fare (updated) ------------------ */
/* prospective: 1 => compute fare for next-seat (booked+1), 0 => current */
int compute_fare(Graph *g, Flight *f,
                 int passenger_age, int is_student,
                 HashTable *ht,
                 int prospective)
{
    (void)ht; /* currently not using points for redemption */

    int distance = compute_distance(g, f->srcIdx, f->destIdx);
    if (distance == INF) return -1;

    /* base rate per km — increased so distance matters */
    const double base_per_km = 0.30; /* tuneable */
    double base = distance * base_per_km;

    /* available seats ratio after prospective booking */
    int booked_after = f->booked + (prospective ? 1 : 0);
    double available_ratio = (f->capacity > 0) ? ((double)(f->capacity - booked_after) / (double)f->capacity) : 0.0;
    if (available_ratio < 0.0) available_ratio = 0.0;

    /* demand multiplier (non-linear feel) */
    const double alpha = 2.0;
    double demand_mult = 1.0 + alpha * (1.0 - available_ratio); /* when available_ratio small -> big multiplier */

    /* peak multiplier */
    double peak = peak_multiplier(f->depart_time);

    /* discounts / passenger multipliers */
    double passenger_mult = 1.0;
    if (passenger_age < 12) passenger_mult *= 0.5; /* child 50% */
    if (passenger_age >= 60) passenger_mult *= 0.85; /* senior 15% off */
    if (is_student) passenger_mult *= 0.90; /* 10% off */

    double price = base * demand_mult * peak * passenger_mult;

    int fare = (int)ceil(price);
    if (fare < 100) fare = 100;
    return fare;
}

/* ------------------ booking (updated awarding points) ------------------ */

int book_ticket(FlightDB *db, HashTable *ht, Graph *g, int flightId,
                PassengerID pid, const char *name, const char *phone,
                int passenger_age, int is_student)
{
    Flight *f = flightdb_get(db, flightId);
    if (!f) { printf("Flight not found\n"); return -1; }

    if (!ht_find(ht, pid)) ht_insert(ht, pid, name, phone);

    /* compute fare for the next seat (prospective) so bookings later are costlier */
    int fare = compute_fare(g, f, passenger_age, is_student, ht, 1);
    if (fare < 0) { printf("Cannot compute fare\n"); return -2; }

    printf("Fare quoted: %d\n", fare);

    if (f->booked < f->capacity) {
        f->booked++;

        ht_update_booking(ht, pid, f->id, f->name, f->booked, f->depart_time);

        /* award points proportional to fare (stronger signal) */
        int pts = fare / 5; /* 1 point per 5 currency units */
        ht_add_points(ht, pid, pts);

        printf("Booked P:%d on F:%d seat:%d fare:%d pts_awarded:%d\n", pid, f->id, f->booked, fare, pts);
        return 0;
    }

    /* full — enqueue using current points as priority */
    int cur_pts = ht_get_points(ht, pid);
    queue_enqueue(&f->waitlist, pid, name, cur_pts);
    printf("Full. Waitlisted P:%d on F:%d (pts=%d)\n", pid, f->id, cur_pts);
    return 1;
}

/* ------------------ cancellation (unchanged except promotion uses points) ------------------ */

int cancel_ticket(FlightDB *db, HashTable *ht, int flightId, PassengerID pid) {
    Flight *f = flightdb_get(db, flightId);
    if (!f) { printf("Flight not found\n"); return -1; }

    Passenger *p = ht_find(ht, pid);
    if (!p || p->booking.flightId != flightId) { printf("No booking\n"); return -1; }

    ht_update_booking(ht, pid, -1, "", -1, "");
    if (f->booked > 0) f->booked--;

    if (!queue_is_empty(&f->waitlist)) {
        PassengerID np; char nn[MAX_NAME_LEN];
        queue_dequeue(&f->waitlist, &np, nn);
        if (!ht_find(ht, np)) ht_insert(ht, np, nn, "NA");
        f->booked++;
        ht_update_booking(ht, np, f->id, f->name, f->booked, f->depart_time);
        /* award a small bonus for promoted passengers */
        ht_add_points(ht, np, 5);
        printf("Promoted P:%d seat:%d (pts:+5)\n", np, f->booked);
    }

    printf("Cancelled P:%d on F:%d\n", pid, flightId);
    return 0;
}

/* ------------------ flight status & route helpers ------------------ */

void flight_status(FlightDB *db, HashTable *ht, int flightId) {
    (void)ht;
    Flight *f = flightdb_get(db, flightId);
    if (!f) { printf("Flight not found\n"); return; }
    printf("Flight ID:%d %s seats:%d/%d wait:%d depart:%s\n",
           f->id, f->name, f->booked, f->capacity, queue_size(&f->waitlist), f->depart_time);
}

/* DFS collector for routes */
static void _collect_paths_dfs(Graph *g, int u, int d, int visited[], int path[], int len, RouteInfo out[], int *out_count, int max_count) {
    visited[u] = 1;
    path[len++] = u;

    if (u == d) {
        if (*out_count < max_count) {
            RouteInfo *r = &out[*out_count];
            r->len = len;
            r->total_distance = 0;
            for (int i = 0; i < len; i++) r->nodes[i] = path[i];
            for (int i = 0; i + 1 < len; i++) r->total_distance += g->adjMatrix[path[i]][path[i+1]];
            (*out_count)++;
        }
    } else {
        for (int v = 0; v < g->numCities; v++) {
            if (g->adjMatrix[u][v] != INF && g->adjMatrix[u][v] != 0 && !visited[v]) {
                _collect_paths_dfs(g, v, d, visited, path, len, out, out_count, max_count);
            }
        }
    }

    visited[u] = 0;
}

void list_routes_with_fares(Graph *g, FlightDB *db, const char *src, const char *dest) {
    int s = getCityIndex(g, src), d = getCityIndex(g, dest);
    if (s == -1 || d == -1) { printf("Invalid cities\n"); return; }

    RouteInfo routes[256];
    int out_count = 0;
    int visited[MAX_CITIES] = {0}, path[MAX_CITIES];
    _collect_paths_dfs(g, s, d, visited, path, 0, routes, &out_count, 256);

    if (out_count == 0) { printf("No routes found\n"); return; }

    /* sort by number of flights (len) ascending */
    for (int i = 0; i < out_count - 1; i++)
        for (int j = i + 1; j < out_count; j++)
            if (routes[i].len > routes[j].len) { RouteInfo t = routes[i]; routes[i] = routes[j]; routes[j] = t; }

    printf("Routes from %s to %s (sorted by number of flights):\n", src, dest);
    for (int i = 0; i < out_count; i++) {
        RouteInfo *r = &routes[i];
        for (int k = 0; k < r->len; k++) {
            printf("%s", g->cityNames[r->nodes[k]]);
            if (k + 1 < r->len) printf(" -> ");
        }
        printf(" | distance: %d km", r->total_distance);

        /* estimate fare using base and demand (approximate) */
        int est_fare = (int)ceil(0.30 * r->total_distance); /* same per-km base used in compute_fare */
        if (est_fare < 100) est_fare = 100;
        printf(" | est_fare: %d\n", est_fare);
    }
}
