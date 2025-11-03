#include "../include/graph.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>


void initGraph(Graph* g){
    g->numCities = 0;
    for(int i = 1; i<= MAX_CITIES ; i++){
        for(int j = 1; j<= MAX_CITIES ; j++){
            g->adjMatrix[i][j]=INF;
        }
    }
}

void addCity(Graph* g, const char* name){
    strcpy(g->cityNames[(g->numCities)++],name);
}


int getCityIndex(Graph *g, const char *name) {
    for (int i = 0; i < g->numCities; i++)
        if (strcmp(g->cityNames[i], name) == 0)
            return i;
    return -1;
}


void addRoute(Graph *g, const char *src, const char *dest, int distance){
    int i = getCityIndex(g, src);
    int j = getCityIndex(g, dest);
    if (i != -1 && j != -1)
        g->adjMatrix[i][j] = g->adjMatrix[j][i] = distance;
}

void displayRoutes(Graph *g) {
    printf("\nAvailable Flight Routes:\n");
    for (int i = 0; i < g->numCities; i++) {
        for (int j = 0; j < g->numCities; j++) {
            if (g->adjMatrix[i][j] != INF)
                printf("%s <-> %s : %d km\n", g->cityNames[i], g->cityNames[j], g->adjMatrix[i][j]);
        }
    }
}

void suggestAlternateRoutes(Graph *g, const char *src, const char *dest) {
    int s = getCityIndex(g, src);
    int d = getCityIndex(g, dest);
    if (s == -1 || d == -1) {
        printf("Invalid cities!\n");
        return;
    }
    printf("\nSuggested alternate route from %s to %s (via BFS simulation):\n", src, dest);
    
    int visited[MAX_CITIES] = {0};
    int parent[MAX_CITIES];
    int queue[MAX_CITIES];
    int front = 0, rear = 0;

    for (int i = 0; i < g->numCities; i++)
        parent[i] = -1;

    visited[s] = 1;
    queue[rear++] = s;

    while(front<rear){
        int u = queue[front++];
            for (int v = 0; v < g->numCities; v++) {
                if(g->adjMatrix[u][v]!=INF && !visited[v]){
                    visited[v]=1;
                    parent[v]=u;
                    queue[rear++]=v;
                    if(v==d)
                        break;
                }
            }
    }

    if (!visited[d]) {
        printf("No alternate routes found from %s to %s.\n", src, dest);
        return;
    }

    int path[MAX_CITIES];
    int count = 0;
    for (int v = d; v != -1; v = parent[v])
        path[count++] = v;

    for (int i = count - 1; i >= 0; i--) {
        printf("%s", g->cityNames[path[i]]);
        if (i != 0) printf(" -> ");
    }
    printf("\n");

    printf("No alternate routes found.\n");
}