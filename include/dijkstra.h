#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

#define HEAP_MAX 512

/* 힙 노드 */
typedef struct {
    int dist;
    int id;
} HeapNode;

/* 최소 힙 */
typedef struct {
    HeapNode data[HEAP_MAX];
    int      size;
} MinHeap;

/* 경로 결과 */
typedef struct {
    int path[MAX_STATIONS];  /* 역 ID 순서 */
    int path_len;
    int total_time;
    int transfers;           /* 환승 횟수 */
    int found;
} PathResult;

/* 함수 선언 */
void       heap_init(MinHeap* h);
void       heap_push(MinHeap* h, int id, int dist);
HeapNode   heap_pop(MinHeap* h);
int        heap_empty(MinHeap* h);

PathResult dijkstra(Graph* g, int start, int end);
void       path_print(Graph* g, PathResult* r, const char* label);

#endif /* DIJKSTRA_H */
