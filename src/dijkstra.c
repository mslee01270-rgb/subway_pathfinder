#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dijkstra.h"

/* ──────────────────────────────────────────
   최소 힙 (우선순위 큐)
   ────────────────────────────────────────── */

void heap_init(MinHeap* h) { h->size = 0; }

static void heap_swap(MinHeap* h, int a, int b) {
    HeapNode tmp = h->data[a];
    h->data[a]   = h->data[b];
    h->data[b]   = tmp;
}

void heap_push(MinHeap* h, int id, int dist) {
    if (h->size >= HEAP_MAX) return;
    h->data[h->size].id   = id;
    h->data[h->size].dist = dist;
    h->size++;

    int i = h->size - 1;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->data[p].dist > h->data[i].dist) {
            heap_swap(h, p, i);
            i = p;
        } else break;
    }
}

HeapNode heap_pop(MinHeap* h) {
    HeapNode top = h->data[0];
    h->data[0] = h->data[--h->size];

    int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, s = i;
        if (l < h->size && h->data[l].dist < h->data[s].dist) s = l;
        if (r < h->size && h->data[r].dist < h->data[s].dist) s = r;
        if (s == i) break;
        heap_swap(h, i, s);
        i = s;
    }
    return top;
}

int heap_empty(MinHeap* h) { return h->size == 0; }

/* ──────────────────────────────────────────
   다익스트라 (최단 시간 경로)
   ────────────────────────────────────────── */

PathResult dijkstra(Graph* g, int start, int end) {
    PathResult result;
    memset(&result, 0, sizeof(result));
    result.found = 0;

    int dist[MAX_STATIONS];
    int prev[MAX_STATIONS];
    int visited[MAX_STATIONS];

    for (int i = 0; i < MAX_STATIONS; i++) {
        dist[i]    = INF;
        prev[i]    = -1;
        visited[i] = 0;
    }

    dist[start] = 0;
    MinHeap h;
    heap_init(&h);
    heap_push(&h, start, 0);

    while (!heap_empty(&h)) {
        HeapNode cur = heap_pop(&h);
        int u = cur.id;

        if (visited[u]) continue;
        visited[u] = 1;

        if (u == end) break;

        EdgeNode* e = g->adj[u];
        while (e) {
            int v    = e->to;
            int newD = dist[u] + e->time;
            if (newD < dist[v]) {
                dist[v] = newD;
                prev[v] = u;
                heap_push(&h, v, newD);
            }
            e = e->next;
        }
    }

    if (dist[end] == INF) return result;

    /* 경로 역추적 */
    result.found      = 1;
    result.total_time = dist[end];

    int path_rev[MAX_STATIONS];
    int len = 0;
    for (int v = end; v != -1; v = prev[v])
        path_rev[len++] = v;

    for (int i = 0; i < len; i++)
        result.path[i] = path_rev[len - 1 - i];
    result.path_len = len;

    /* 환승 횟수 계산 */
    int transfers = 0;
    for (int i = 1; i < len - 1; i++) {
        if (g->stations[result.path[i-1]].line !=
            g->stations[result.path[i]].line)
            transfers++;
    }
    result.transfers = transfers;

    return result;
}

void path_print(Graph* g, PathResult* r, const char* label) {
    if (!r->found) {
        printf("  경로를 찾을 수 없습니다.\n");
        return;
    }
    printf("\n  [%s]\n", label);
    printf("  ");
    for (int i = 0; i < r->path_len; i++) {
        int id = r->path[i];
        printf("%s", g->stations[id].name);
        if (i < r->path_len - 1) printf(" → ");
    }
    printf("\n");
    printf("  총 소요 시간: %d분  |  환승: %d회\n",
           r->total_time, r->transfers);
}
