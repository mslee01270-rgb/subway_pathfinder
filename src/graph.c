#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

/*
 * graph.c - 그래프 (인접 리스트) 구현
 *
 * 지하철 노선을 그래프로 표현한다.
 * 역(Station) = 노드(Node)
 * 노선 연결 = 간선(Edge)
 *
 * 인접 리스트를 사용한 이유:
 * 역이 300개라도 한 역에서 연결된 역은 2~3개뿐이므로
 * 인접 행렬(300x300 배열)보다 메모리 효율이 훨씬 좋다.
 */

/* 그래프 생성 - 모든 인접 리스트를 NULL로 초기화 */
Graph* graph_create(void) {
    Graph* g = (Graph*)calloc(1, sizeof(Graph));
    for (int i = 0; i < MAX_STATIONS; i++)
        g->adj[i] = NULL;
    g->station_count = 0;
    return g;
}

/* 그래프 메모리 해제 - 모든 간선 노드를 순서대로 해제 */
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

/*
 * 역 추가
 * id: 역 고유 번호
 * name: 역 이름
 * line: 노선 번호
 * 이미 존재하는 역이면 추가하지 않는다.
 */
int graph_add_station(Graph* g, int id, const char* name, int line) {
    /* 유효하지 않은 ID 범위 체크 */
    if (id < 0 || id >= MAX_STATIONS) return -1;

    /* 이미 등록된 역이면 스킵 */
    if (g->stations[id].valid) return 0;

    g->stations[id].id    = id;
    g->stations[id].line  = line;
    g->stations[id].valid = 1;
    strncpy(g->stations[id].name, name, MAX_NAME_LEN - 1);
    g->stations[id].name[MAX_NAME_LEN - 1] = '\0'; /* 문자열 끝 보장 */
    g->station_count++;
    return 0;
}

/*
 * 간선 추가 (양방향)
 * from → to, to → from 두 방향 모두 추가
 * 지하철은 양방향 이동이 가능하므로 무방향 그래프로 구현
 */
void graph_add_edge(Graph* g, int from, int to, int time, int line) {
    /* 유효하지 않은 ID 체크 */
    if (from < 0 || from >= MAX_STATIONS) return;
    if (to   < 0 || to   >= MAX_STATIONS) return;
    /* 소요 시간이 음수인 경우 체크 */
    if (time < 0) return;

    /* from → to 방향 간선 추가 */
    EdgeNode* e1 = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (!e1) return; /* 메모리 할당 실패 체크 */
    e1->to   = to;
    e1->time = time;
    e1->line = line;
    e1->next = g->adj[from];
    g->adj[from] = e1;

    /* to → from 방향 간선 추가 (양방향) */
    EdgeNode* e2 = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (!e2) return; /* 메모리 할당 실패 체크 */
    e2->to   = from;
    e2->time = time;
    e2->line = line;
    e2->next = g->adj[to];
    g->adj[to] = e2;
}

/* 그래프 전체 출력 - 디버깅용 */
void graph_print(Graph* g) {
    printf("\n=== 노선 그래프 ===\n");
    for (int i = 0; i < MAX_STATIONS; i++) {
        if (!g->stations[i].valid) continue;
        printf("[%3d] %-20s (%d호선) → ",
               i, g->stations[i].name, g->stations[i].line);
        EdgeNode* e = g->adj[i];
        while (e) {
            printf("%s(%d분) ", g->stations[e->to].name, e->time);
            e = e->next;
        }
        printf("\n");
    }
}

/* 전체 역 수 반환 */
int graph_station_count(Graph* g) { return g->station_count; }
