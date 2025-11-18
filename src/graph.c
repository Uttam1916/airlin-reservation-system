#include "../include/graph.h"
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
        if (strcmp(g->cityNames[i], name) == 0)
            return i;

    return -1;
}

void addRoute(Graph *g, const char *src, const char *dest, int distance) {
    int i = getCityIndex(g, src);
    int j = getCityIndex(g, dest);

    if (i == -1 || j == -1) return;

    g->adjMatrix[i][j] = distance;
    g->adjMatrix[j][i] = distance;
}

void displayRoutes(Graph *g) {
    printf("\nRoutes:\n");

    for (int i = 0; i < g->numCities; i++) {
        for (int j = i + 1; j < g->numCities; j++) {
            if (g->adjMatrix[i][j] != INF && g->adjMatrix[i][j] != 0) {
                printf("%s <-> %s : %d km\n",
                       g->cityNames[i],
                       g->cityNames[j],
                       g->adjMatrix[i][j]);
            }
        }
    }
}

/* BFS shortest-by-hops path */
void suggestAlternateRoutes(Graph *g, const char *src, const char *dest) {
    int s = getCityIndex(g, src);
    int d = getCityIndex(g, dest);

    if (s == -1 || d == -1) {
        printf("Invalid cities\n");
        return;
    }

    int visited[MAX_CITIES] = {0};
    int parent[MAX_CITIES];
    int queue[MAX_CITIES];
    int front = 0, rear = 0;

    for (int i = 0; i < MAX_CITIES; i++)
        parent[i] = -1;

    visited[s] = 1;
    queue[rear++] = s;

    while (front < rear) {
        int u = queue[front++];

        if (u == d) break;

        for (int v = 0; v < g->numCities; v++) {
            if (g->adjMatrix[u][v] != INF &&
                g->adjMatrix[u][v] != 0 &&
                !visited[v])
            {
                visited[v] = 1;
                parent[v] = u;
                queue[rear++] = v;
            }
        }
    }

    if (!visited[d]) {
        printf("No route found\n");
        return;
    }

    int path[MAX_CITIES], count = 0;
    for (int v = d; v != -1; v = parent[v])
        path[count++] = v;

    for (int i = count - 1; i >= 0; i--) {
        printf("%s", g->cityNames[path[i]]);
        if (i) printf(" -> ");
    }
    printf("\n");
}

/* DFS helper for route enumeration */
static void dfs_all_paths(Graph *g, int u, int d,
                          int visited[], int path[], int len)
{
    visited[u] = 1;
    path[len++] = u;

    if (u == d) {
        for (int i = 0; i < len; i++) {
            printf("%s", g->cityNames[path[i]]);
            if (i + 1 < len) printf(" -> ");
        }
        printf("\n");
    } else {
        for (int v = 0; v < g->numCities; v++) {
            if (g->adjMatrix[u][v] != INF &&
                g->adjMatrix[u][v] != 0 &&
                !visited[v])
            {
                dfs_all_paths(g, v, d, visited, path, len);
            }
        }
    }

    visited[u] = 0;
}

void find_all_paths(Graph *g, const char *src, const char *dest) {
    int s = getCityIndex(g, src);
    int d = getCityIndex(g, dest);

    if (s == -1 || d == -1) {
        printf("Invalid cities\n");
        return;
    }

    int visited[MAX_CITIES] = {0};
    int path[MAX_CITIES];

    dfs_all_paths(g, s, d, visited, path, 0);
}

void dijkstra_shortest_path(Graph *g, const char *src, const char *dest) {
    int s = getCityIndex(g, src);
    int d = getCityIndex(g, dest);

    if (s == -1 || d == -1) {
        printf("Invalid cities\n");
        return;
    }

    int dist[MAX_CITIES];
    int prev[MAX_CITIES];
    int used[MAX_CITIES];

    for (int i = 0; i < g->numCities; i++) {
        dist[i] = INF;
        prev[i] = -1;
        used[i] = 0;
    }

    dist[s] = 0;

    for (int it = 0; it < g->numCities; it++) {
        int u = -1;
        int best = INF;

        for (int i = 0; i < g->numCities; i++) {
            if (!used[i] && dist[i] < best) {
                best = dist[i];
                u = i;
            }
        }

        if (u == -1) break;

        used[u] = 1;

        for (int v = 0; v < g->numCities; v++) {
            if (g->adjMatrix[u][v] != INF &&
                g->adjMatrix[u][v] > 0)
            {
                if (dist[u] + g->adjMatrix[u][v] < dist[v]) {
                    dist[v] = dist[u] + g->adjMatrix[u][v];
                    prev[v] = u;
                }
            }
        }
    }

    if (dist[d] == INF) {
        printf("No route found\n");
        return;
    }

    int path[MAX_CITIES];
    int count = 0;

    for (int v = d; v != -1; v = prev[v])
        path[count++] = v;

    for (int i = count - 1; i >= 0; i--) {
        printf("%s", g->cityNames[path[i]]);
        if (i) printf(" -> ");
    }

    printf(" : %d km\n", dist[d]);
}
