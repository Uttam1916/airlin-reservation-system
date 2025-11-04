#ifndef GRAPH_H
#define GRAPH_H

#include "utils.h"

typedef struct {
    int numCities;
    int adjMatrix[MAX_CITIES][MAX_CITIES];
    char cityNames[MAX_CITIES][MAX_NAME_LEN];
} Graph;

void initGraph(Graph *g);
int  addCity(Graph *g, const char *name);
int  getCityIndex(Graph *g, const char *name);
void addRoute(Graph *g, const char *src, const char *dest, int distance);
void displayRoutes(Graph *g);
void suggestAlternateRoutes(Graph *g, const char *src, const char *dest);

#endif
