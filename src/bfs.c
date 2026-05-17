#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bfs.h"

/* ──────────────────────────────────────────
   BFS: 최소 환승 경로 탐색
   환승을 가중치로 보고 BFS로 탐색
   ────────────────────────────────────────── */

#define BFS_QUEUE_MAX (MAX_STATIONS * 4)

typedef struct {
    int station;
    int line;       /* 현재 탑승 노선 */
    int transfers;
    int time;
    int prev_idx;   /* 이전 노드 인덱스 */
} BFSNode;

PathResult bfs_min_transfer(Graph* g, int start, int end) {
    PathResult result;
    memset(&result, 0, sizeof(result));
    result.found = 0;

    BFSNode queue[BFS_QUEUE_MAX];
    int     qfront = 0, qback = 0;

    /* visited[station][line] */
    int visited[MAX_STATIONS][12];
    memset(visited, 0, sizeof(visited));

    /* 시작역의 모든 연결 노선으로 초기 삽입 */
    EdgeNode* e = g->adj[start];
    while (e) {
        if (!visited[start][e->line]) {
            visited[start][e->line] = 1;
            BFSNode node;
            node.station   = start;
            node.line      = e->line;
            node.transfers = 0;
            node.time      = 0;
            node.prev_idx  = -1;
            queue[qback++] = node;
        }
        e = e->next;
    }

    int found_idx = -1;

    while (qfront < qback) {
        BFSNode cur = queue[qfront];
        int cur_idx = qfront;
        qfront++;

        if (cur.station == end) {
            found_idx = cur_idx;
            break;
        }

        EdgeNode* edge = g->adj[cur.station];
        while (edge) {
            int next     = edge->to;
            int next_line = edge->line;
            int extra_transfer = (next_line != cur.line) ? 1 : 0;
            int new_transfers  = cur.transfers + extra_transfer;

            if (!visited[next][next_line]) {
                visited[next][next_line] = 1;
                if (qback < BFS_QUEUE_MAX) {
                    BFSNode node;
                    node.station   = next;
                    node.line      = next_line;
                    node.transfers = new_transfers;
                    node.time      = cur.time + edge->time;
                    node.prev_idx  = cur_idx;
                    queue[qback++] = node;
                }
            }
            edge = edge->next;
        }
    }

    if (found_idx == -1) return result;

    /* 경로 역추적 */
    int path_rev[MAX_STATIONS];
    int len = 0;
    int idx = found_idx;
    while (idx != -1) {
        path_rev[len++] = queue[idx].station;
        idx = queue[idx].prev_idx;
    }

    result.found      = 1;
    result.total_time = queue[found_idx].time;
    result.transfers  = queue[found_idx].transfers;
    result.path_len   = len;
    for (int i = 0; i < len; i++)
        result.path[i] = path_rev[len - 1 - i];

    return result;
}
