#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dijkstra.h"

/*
 * dijkstra.c - 최소 힙 + 다익스트라 알고리즘 구현
 *
 * 출발역에서 도착역까지 소요 시간이 가장 짧은 경로를 탐색한다.
 *
 * 최소 힙을 사용하는 이유:
 * 다익스트라는 매 단계마다 거리가 가장 짧은 역을 선택해야 한다.
 * 단순 배열로 찾으면 O(N)이지만 최소 힙을 쓰면 O(log N)으로 빠르다.
 *
 * 힙 구조: 배열 기반 완전 이진 트리
 * - 부모 인덱스: (i-1)/2
 * - 왼쪽 자식: 2*i+1
 * - 오른쪽 자식: 2*i+2
 */

/* ── 최소 힙 ── */

/* 힙 초기화 */
void heap_init(MinHeap* h) { h->size = 0; }

/* 힙 원소 교환 */
static void heap_swap(MinHeap* h, int a, int b) {
    HeapNode tmp = h->data[a];
    h->data[a]   = h->data[b];
    h->data[b]   = tmp;
}

/*
 * 힙에 원소 삽입 (sift-up)
 * 1. 맨 뒤에 삽입
 * 2. 부모보다 작으면 교환하며 위로 올라감
 */
void heap_push(MinHeap* h, int id, int dist) {
    /* 힙이 가득 찬 경우 */
    if (h->size >= HEAP_MAX) return;

    h->data[h->size].id   = id;
    h->data[h->size].dist = dist;
    h->size++;

    /* sift-up: 부모보다 작으면 위로 올림 */
    int i = h->size - 1;
    while (i > 0) {
        int p = (i - 1) / 2; /* 부모 인덱스 */
        if (h->data[p].dist > h->data[i].dist) {
            heap_swap(h, p, i);
            i = p;
        } else break;
    }
}

/*
 * 힙에서 최솟값 꺼내기 (sift-down)
 * 1. 루트(최솟값) 꺼냄
 * 2. 맨 뒤 원소를 루트로 이동
 * 3. 자식보다 크면 교환하며 아래로 내려감
 */
HeapNode heap_pop(MinHeap* h) {
    HeapNode top = h->data[0]; /* 최솟값 저장 */
    h->data[0] = h->data[--h->size]; /* 맨 뒤를 루트로 */

    /* sift-down: 자식보다 크면 아래로 내림 */
    int i = 0;
    while (1) {
        int l = 2*i+1; /* 왼쪽 자식 */
        int r = 2*i+2; /* 오른쪽 자식 */
        int s = i;     /* 가장 작은 인덱스 */

        if (l < h->size && h->data[l].dist < h->data[s].dist) s = l;
        if (r < h->size && h->data[r].dist < h->data[s].dist) s = r;
        if (s == i) break; /* 더 이상 내려갈 필요 없음 */

        heap_swap(h, i, s);
        i = s;
    }
    return top;
}

/* 힙이 비었는지 확인 */
int heap_empty(MinHeap* h) { return h->size == 0; }

/* ── 다익스트라 알고리즘 ── */

/*
 * 다익스트라: 출발역에서 도착역까지 최단 시간 경로 탐색
 *
 * 동작 원리:
 * 1. 출발역 거리 = 0, 나머지 = 무한대(INF)로 초기화
 * 2. 최소 힙에서 거리가 가장 짧은 역을 꺼냄
 * 3. 연결된 역들의 거리를 업데이트
 * 4. 도착역에 도달하면 종료
 */
PathResult dijkstra(Graph* g, int start, int end) {
    PathResult result;
    memset(&result, 0, sizeof(result));
    result.found = 0;

    /* 예외 처리: 유효하지 않은 역 ID */
    if (start < 0 || start >= MAX_STATIONS) return result;
    if (end   < 0 || end   >= MAX_STATIONS) return result;
    if (!g->stations[start].valid || !g->stations[end].valid) return result;
    if (start == end) return result;

    int dist[MAX_STATIONS];    /* 출발역까지 최단 거리 */
    int prev[MAX_STATIONS];    /* 경로 역추적용 이전 역 */
    int visited[MAX_STATIONS]; /* 방문 여부 */

    /* 초기화 */
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

        /* 이미 방문한 역이면 스킵 */
        if (visited[u]) continue;
        visited[u] = 1;

        /* 도착역에 도달하면 종료 */
        if (u == end) break;

        /* 인접한 역들의 거리 업데이트 */
        EdgeNode* e = g->adj[u];
        while (e) {
            int v    = e->to;
            int newD = dist[u] + e->time;

            /* 더 짧은 경로 발견 시 업데이트 */
            if (newD < dist[v]) {
                dist[v] = newD;
                prev[v] = u;
                heap_push(&h, v, newD);
            }
            e = e->next;
        }
    }

    /* 경로가 없는 경우 */
    if (dist[end] == INF) return result;

    /* 경로 역추적 (도착역 → 출발역 순서로 저장 후 뒤집기) */
    result.found      = 1;
    result.total_time = dist[end];

    int path_rev[MAX_STATIONS];
    int len = 0;
    for (int v = end; v != -1; v = prev[v]) {
        if (len >= MAX_STATIONS) break; /* 무한루프 방지 */
        path_rev[len++] = v;
    }

    /* 역순으로 저장 */
    for (int i = 0; i < len; i++)
        result.path[i] = path_rev[len - 1 - i];
    result.path_len = len;

    /* 환승 횟수 계산: 이전 역과 노선이 다르면 환승 */
    int transfers = 0;
    for (int i = 1; i < len - 1; i++) {
        if (g->stations[result.path[i-1]].line !=
            g->stations[result.path[i]].line)
            transfers++;
    }
    result.transfers = transfers;

    return result;
}

/* 경로 출력 */
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
