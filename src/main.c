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

/* '역' UTF-8(EC 97 AD) 자동 제거 */
static void remove_station_suffix(char* name) {
    int len = (int)strlen(name);
    if (len >= 3 &&
        (unsigned char)name[len-3] == 0xEC &&
        (unsigned char)name[len-2] == 0x97 &&
        (unsigned char)name[len-1] == 0xAD)
        name[len-3] = '\0';
}

int main(void) {
    Graph*        g = graph_create();
    StationHash*  h = hash_create();
    HistoryStack* s = history_create();

    ui_clear();
    ui_print_title();
    printf("데이터 로드 중...\n");
    if (!load_subway_data(g, h))
        load_default_data(g, h);

    while (1) {
        ui_clear();
        ui_print_title();
        ui_print_menu();

        int choice;
        if (scanf("%d", &choice) != 1) { getchar(); continue; }

        if (choice == 1) {
            char from_name[MAX_NAME_LEN];
            char to_name[MAX_NAME_LEN];

            printf("\n  * 역 이름만 입력하세요 (예: 강남, 홍대입구, 소사)\n");
            printf("  * '역'을 붙여도 자동으로 처리됩니다\n\n");
            printf("출발역 입력: ");
            scanf("%63s", from_name);
            printf("도착역 입력: ");
            scanf("%63s", to_name);

            /* '역' 자동 제거 */
            remove_station_suffix(from_name);
            remove_station_suffix(to_name);

            int from_id = hash_find(h, from_name);
            int to_id   = hash_find(h, to_name);

            if (from_id == -1) {
                printf("\n[!] '%s' 역을 찾을 수 없습니다.\n", from_name);
                printf("  → 3번 메뉴에서 전체 역 목록을 확인하세요\n");
                printf("아무 키나 누르세요...\n"); getchar(); getchar();
                continue;
            }
            if (to_id == -1) {
                printf("\n[!] '%s' 역을 찾을 수 없습니다.\n", to_name);
                printf("  → 3번 메뉴에서 전체 역 목록을 확인하세요\n");
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

            if (dijk.found)
                history_push(s, from_name, to_name,
                             dijk.total_time, dijk.transfers);

            printf("\n아무 키나 누르세요...\n"); getchar(); getchar();

        } else if (choice == 2) {
            ui_clear();
            ui_print_title();
            history_print(s);
            printf("\n아무 키나 누르세요...\n"); getchar(); getchar();

        } else if (choice == 3) {
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

    graph_destroy(g);
    hash_destroy(h);
    history_destroy(s);
    return 0;
}
