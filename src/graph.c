#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

Graph* graph_create(void) {
    Graph* g = (Graph*)calloc(1, sizeof(Graph));
    for (int i = 0; i < MAX_STATIONS; i++)
        g->adj[i] = NULL;
    g->station_count = 0;
    return g;
}

void graph_destroy(Graph* g) {
    if (!g) return;
    for (int i = 0; i < MAX_STATIONS; i++) {
        EdgeNode* cur = g->adj[i];
        while (cur) {
            EdgeNode* nxt = cur->next;
            free(cur);
            cur = nxt;
        }
    }
    free(g);
}

int graph_add_station(Graph* g, int id, const char* name, int line) {
    if (id < 0 || id >= MAX_STATIONS) return -1;
    if (g->stations[id].valid) return 0; /* 이미 존재 */
    g->stations[id].id    = id;
    g->stations[id].line  = line;
    g->stations[id].valid = 1;
    strncpy(g->stations[id].name, name, MAX_NAME_LEN - 1);
    g->station_count++;
    return 0;
}

void graph_add_edge(Graph* g, int from, int to, int time, int line) {
    if (from < 0 || from >= MAX_STATIONS) return;
    if (to   < 0 || to   >= MAX_STATIONS) return;

    /* 양방향 추가 */
    EdgeNode* e1 = (EdgeNode*)malloc(sizeof(EdgeNode));
    e1->to   = to;
    e1->time = time;
    e1->line = line;
    e1->next = g->adj[from];
    g->adj[from] = e1;

    EdgeNode* e2 = (EdgeNode*)malloc(sizeof(EdgeNode));
    e2->to   = from;
    e2->time = time;
    e2->line = line;
    e2->next = g->adj[to];
    g->adj[to] = e2;
}

void graph_print(Graph* g) {
    printf("\n=== 노선 그래프 ===\n");
    for (int i = 0; i < MAX_STATIONS; i++) {
        if (!g->stations[i].valid) continue;
        printf("[%3d] %-20s (%-1d호선) → ", i,
               g->stations[i].name, g->stations[i].line);
        EdgeNode* e = g->adj[i];
        while (e) {
            printf("%s(%d분) ", g->stations[e->to].name, e->time);
            e = e->next;
        }
        printf("\n");
    }
}

int graph_station_count(Graph* g) { return g->station_count; }
