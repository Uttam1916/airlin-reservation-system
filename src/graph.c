#include "graph.h"
#include <stdio.h>
#include <string.h>

void initGraph(Graph *g) {
    g->numCities = 0;
    for (int i = 0; i < MAX_CITIES; i++)
        for (int j = 0; j < MAX_CITIES; j++)
            g->adjMatrix[i][j] = (i == j) ? 0 : INF;
}

int addCity(Graph *g, const char *name) {
    if (g->numCities >= MAX_CITIES) return -1;
    strncpy(g->cityNames[g->numCities], name, MAX_NAME_LEN - 1);
    g->cityNames[g->numCities][MAX_NAME_LEN - 1] = '\0';
    return g->numCities++;
}

int getCityIndex(Graph *g, const char *name) {
    for (int i = 0; i < g->numCities; i++)
        if (strcmp(g->cityNames[i], name) == 0) return i;
    return -1;
}

void addRoute(Graph *g, const char *src, const char *dest, int distance) {
    int i = getCityIndex(g, src), j = getCityIndex(g, dest);
    if (i == -1 || j == -1) return;
    g->adjMatrix[i][j] = g->adjMatrix[j][i] = distance;
}

void displayRoutes(Graph *g) {
    printf("\nRoutes:\n");
    for (int i = 0; i < g->numCities; i++)
        for (int j = i + 1; j < g->numCities; j++)
            if (g->adjMatrix[i][j] != INF && g->adjMatrix[i][j] != 0)
                printf("%s <-> %s : %d km\n", g->cityNames[i], g->cityNames[j], g->adjMatrix[i][j]);
}

void suggestAlternateRoutes(Graph *g, const char *src, const char *dest) {
    int s = getCityIndex(g, src), d = getCityIndex(g, dest);
    if (s == -1 || d == -1) { printf("Invalid cities\n"); return; }

    int visited[MAX_CITIES] = {0}, parent[MAX_CITIES], q[MAX_CITIES];
    int front = 0, rear = 0, found = 0;
    for (int i = 0; i < g->numCities; i++) parent[i] = -1;

    visited[s] = 1; q[rear++] = s;
    while (front < rear && !found) {
        int u = q[front++];
        for (int v = 0; v < g->numCities; v++) {
            if (g->adjMatrix[u][v] != INF && g->adjMatrix[u][v] != 0 && !visited[v]) {
                visited[v] = 1; parent[v] = u; q[rear++] = v;
                if (v == d) { found = 1; break; }
            }
        }
    }
    if (!visited[d]) { printf("No route found\n"); return; }

    int path[MAX_CITIES], cnt = 0;
    for (int v = d; v != -1; v = parent[v]) path[cnt++] = v;
    for (int i = cnt - 1; i >= 0; i--) {
        printf("%s", g->cityNames[path[i]]);
        if (i) printf(" -> ");
    }
    printf("\n");
}
