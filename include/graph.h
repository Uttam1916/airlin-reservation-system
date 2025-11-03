#ifndef GRAPH_H
#define GRAPH_H

#define MAX_CITIES 20
#define INF 999

typedef struct{
    int numCities;
    int cityNames[MAX_CITIES][30];
    int adjMatrix[MAX_CITIES][MAX_CITIES];
} Graph;

void initGraph(Graph *g);
void addCity(Graph *g, const char *name);
void addRoute(Graph *g, const char *src, const char *dest, int distance);
int getCityIndex(Graph *g, const char *name);
void displayRoutes(Graph *g);
void suggestAlternateRoutes(Graph *g, const char *src, const char *dest);


#endif