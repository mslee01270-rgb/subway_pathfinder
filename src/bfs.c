#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bfs.h"

/*
 * bfs.c - BFS(너비 우선 탐색) 최소 환승 경로 탐색
 *
 * 출발역에서 도착역까지 환승 횟수가 가장 적은 경로를 탐색한다.
 *
 * BFS를 사용하는 이유:
 * BFS는 가까운 노드부터 순서대로 탐색한다.
 * 환승을 가중치로 보면 같은 노선 = 0, 환승 = 1이 된다.
 * 가중치가 0 또는 1로 동일할 때 BFS가 최소 환승을 보장한다.
 *
 * 다익스트라와의 차이:
 * - 다익스트라: 소요 시간 최소 (최소 힙 사용)
 * - BFS: 환승 횟수 최소 (큐 사용)
 * 같은 출발/도착이라도 결과가 다를 수 있다.
 */

#define BFS_QUEUE_MAX (MAX_STATIONS * 4)

/* BFS 탐색에 사용하는 노드 구조체 */
typedef struct {
    int station;    /* 현재 역 ID */
    int line;       /* 현재 탑승 노선 */
    int transfers;  /* 현재까지 환승 횟수 */
    int time;       /* 현재까지 소요 시간 */
    int prev_idx;   /* 경로 역추적용 이전 노드 인덱스 */
} BFSNode;

/*
 * BFS 최소 환승 경로 탐색
 *
 * 동작 원리:
 * 1. 출발역의 모든 연결 노선을 큐에 넣기
 * 2. 큐에서 노드를 꺼내서 연결된 역을 큐에 넣기
 * 3. 다른 노선으로 이동하면 환승 횟수 +1
 * 4. 도착역에 도달하면 종료 (BFS 특성상 첫 도달 = 최소 환승)
 */
PathResult bfs_min_transfer(Graph* g, int start, int end) {
    PathResult result;
    memset(&result, 0, sizeof(result));
    result.found = 0;

    /* 예외 처리: 유효하지 않은 역 ID */
    if (start < 0 || start >= MAX_STATIONS) return result;
    if (end   < 0 || end   >= MAX_STATIONS) return result;
    if (!g->stations[start].valid || !g->stations[end].valid) return result;
    if (start == end) return result;

    BFSNode queue[BFS_QUEUE_MAX]; /* BFS 탐색 큐 */
    int     qfront = 0, qback = 0;

    /* visited[역][노선]: 같은 역이라도 다른 노선으로 방문 가능 */
    int visited[MAX_STATIONS][12];
    memset(visited, 0, sizeof(visited));

    /* 출발역에 연결된 모든 노선을 큐에 삽입 */
    EdgeNode* e = g->adj[start];
    while (e) {
        int ln = e->line;
        if (ln >= 0 && ln < 12 && !visited[start][ln]) {
            visited[start][ln] = 1;
            BFSNode node;
            node.station   = start;
            node.line      = ln;
            node.transfers = 0;
            node.time      = 0;
            node.prev_idx  = -1;
            queue[qback++] = node;
        }
        e = e->next;
    }

    int found_idx = -1;

    /* BFS 탐색 시작 */
    while (qfront < qback) {
        BFSNode cur = queue[qfront];
        int cur_idx = qfront;
        qfront++;

        /* 도착역에 도달하면 종료 */
        if (cur.station == end) {
            found_idx = cur_idx;
            break;
        }

        /* 연결된 역들을 큐에 삽입 */
        EdgeNode* edge = g->adj[cur.station];
        while (edge) {
            int next      = edge->to;
            int next_line = edge->line;

            /* 유효하지 않은 노선 번호 체크 */
            if (next_line < 0 || next_line >= 12) {
                edge = edge->next;
                continue;
            }

            /* 노선이 바뀌면 환승 횟수 +1 */
            int extra_transfer = (next_line != cur.line) ? 1 : 0;
            int new_transfers  = cur.transfers + extra_transfer;

            if (!visited[next][next_line]) {
                visited[next][next_line] = 1;

                /* 큐가 가득 찬 경우 체크 */
                if (qback >= BFS_QUEUE_MAX) break;

                BFSNode node;
                node.station   = next;
                node.line      = next_line;
                node.transfers = new_transfers;
                node.time      = cur.time + edge->time;
                node.prev_idx  = cur_idx;
                queue[qback++] = node;
            }
            edge = edge->next;
        }
    }

    /* 경로가 없는 경우 */
    if (found_idx == -1) return result;

    /* 경로 역추적 (도착역 → 출발역 순서로 저장 후 뒤집기) */
    int path_rev[MAX_STATIONS];
    int len = 0;
    int idx = found_idx;
    while (idx != -1 && len < MAX_STATIONS) {
        path_rev[len++] = queue[idx].station;
        idx = queue[idx].prev_idx;
    }

    result.found      = 1;
    result.total_time = queue[found_idx].time;
    result.transfers  = queue[found_idx].transfers;
    result.path_len   = len;

    /* 역순으로 저장 */
    for (int i = 0; i < len; i++)
        result.path[i] = path_rev[len - 1 - i];

    return result;
}
