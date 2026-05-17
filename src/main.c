#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "dijkstra.h"
#include "bfs.h"
#include "station.h"
#include "history.h"
#include "file_io.h"
#include "ui.h"

int main(void) {
    /* 초기화 */
    Graph*        g = graph_create();
    StationHash*  h = hash_create();
    HistoryStack* s = history_create();

    /* 데이터 로드 */
    ui_clear();
    ui_print_title();
    printf("데이터 로드 중...\n");
    if (!load_subway_data(g, h))
        load_default_data(g, h);

    /* 메인 루프 */
    while (1) {
        ui_clear();
        ui_print_title();
        ui_print_menu();

        int choice;
        if (scanf("%d", &choice) != 1) { getchar(); continue; }

        if (choice == 1) {
            /* ── 경로 탐색 ── */
            char from_name[MAX_NAME_LEN];
            char to_name[MAX_NAME_LEN];

            printf("\n출발역 입력: ");
            scanf("%63s", from_name);
            printf("도착역 입력: ");
            scanf("%63s", to_name);

            int from_id = hash_find(h, from_name);
            int to_id   = hash_find(h, to_name);

            if (from_id == -1) {
                printf("\n[!] '%s' 역을 찾을 수 없습니다.\n", from_name);
                printf("아무 키나 누르세요...\n"); getchar(); getchar();
                continue;
            }
            if (to_id == -1) {
                printf("\n[!] '%s' 역을 찾을 수 없습니다.\n", to_name);
                printf("아무 키나 누르세요...\n"); getchar(); getchar();
                continue;
            }
            if (from_id == to_id) {
                printf("\n[!] 출발역과 도착역이 같습니다.\n");
                printf("아무 키나 누르세요...\n"); getchar(); getchar();
                continue;
            }

            PathResult dijk = dijkstra(g, from_id, to_id);
            PathResult bfs  = bfs_min_transfer(g, from_id, to_id);

            ui_print_result(g, &dijk, &bfs, from_name, to_name);

            /* 검색 기록 저장 */
            if (dijk.found)
                history_push(s, from_name, to_name,
                             dijk.total_time, dijk.transfers);

            printf("\n아무 키나 누르세요...\n"); getchar(); getchar();

        } else if (choice == 2) {
            /* ── 검색 기록 ── */
            ui_clear();
            ui_print_title();
            history_print(s);
            printf("\n아무 키나 누르세요...\n"); getchar(); getchar();

        } else if (choice == 3) {
            /* ── 역 목록 ── */
            ui_clear();
            ui_print_title();
            printf("=== 전체 역 목록 ===\n");
            for (int i = 0; i < MAX_STATIONS; i++) {
                if (g->stations[i].valid)
                    printf("  [%3d] %-20s (%d호선)\n",
                           i, g->stations[i].name, g->stations[i].line);
            }
            printf("\n아무 키나 누르세요...\n"); getchar(); getchar();

        } else if (choice == 4) {
            printf("\n종료합니다.\n");
            break;
        }
    }

    /* 메모리 해제 */
    graph_destroy(g);
    hash_destroy(h);
    history_destroy(s);
    return 0;
}
