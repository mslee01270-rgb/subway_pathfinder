#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "graph.h"
#include "dijkstra.h"
#include "bfs.h"
#include "station.h"
#include "history.h"
#include "file_io.h"
#include "ui.h"

/* UTF-8에서 '역'(EC 97 AD) 자동 제거 */
static void remove_station_suffix(char* name) {
    int len = (int)strlen(name);
    if (len >= 3 &&
        (unsigned char)name[len-3] == 0xEC &&
        (unsigned char)name[len-2] == 0x97 &&
        (unsigned char)name[len-1] == 0xAD)
        name[len-3] = '\0';
}

/*
 * 유사 역 추천
 * UTF-8 바이트 단위로 앞부분 일치 여부 비교
 * 입력값 길이만큼만 비교해서 부분 일치 찾기
 */
static void suggest_station(Graph* g, const char* input) {
    int found = 0;
    int input_len = (int)strlen(input);
    if (input_len == 0) return;

    printf("  혹시 이 역을 찾으셨나요?\n");
    for (int i = 0; i < MAX_STATIONS; i++) {
        if (!g->stations[i].valid) continue;
        const char* name = g->stations[i].name;
        int name_len = (int)strlen(name);
        if (name_len < input_len) continue;

        /* 앞부분 바이트 비교 */
        int match = 1;
        for (int j = 0; j < input_len; j++) {
            if (name[j] != input[j]) { match = 0; break; }
        }
        if (match) {
            printf("  → %s (%d호선)\n",
                   g->stations[i].name, g->stations[i].line);
            found++;
            if (found >= 5) break;
        }
    }
    if (!found)
        printf("  (유사한 역 없음. 3번 메뉴에서 전체 목록 확인)\n");
}

/* 결과를 txt 파일로 저장 */
static void save_result_to_file(Graph* g, PathResult* dijk, PathResult* bfs,
                                 const char* from, const char* to) {
#ifdef _WIN32
    system("mkdir data 2>nul");
#else
    system("mkdir -p data");
#endif

    FILE* f = fopen("data/result.txt", "a");
    if (!f) { printf("  [!] 파일 저장 실패\n"); return; }

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    fprintf(f, "========================================\n");
    fprintf(f, "검색 시각: %04d-%02d-%02d %02d:%02d:%02d\n",
            t->tm_year+1900, t->tm_mon+1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    fprintf(f, "구간: %s -> %s\n\n", from, to);

    fprintf(f, "[최단 시간 경로 - 다익스트라]\n");
    if (dijk->found) {
        for (int i = 0; i < dijk->path_len; i++) {
            fprintf(f, "%s", g->stations[dijk->path[i]].name);
            if (i < dijk->path_len - 1) fprintf(f, " -> ");
        }
        fprintf(f, "\n총 %d분 | 환승 %d회\n\n", dijk->total_time, dijk->transfers);
    } else fprintf(f, "경로 없음\n\n");

    fprintf(f, "[최소 환승 경로 - BFS]\n");
    if (bfs->found) {
        for (int i = 0; i < bfs->path_len; i++) {
            fprintf(f, "%s", g->stations[bfs->path[i]].name);
            if (i < bfs->path_len - 1) fprintf(f, " -> ");
        }
        fprintf(f, "\n총 %d분 | 환승 %d회\n\n", bfs->total_time, bfs->transfers);
    } else fprintf(f, "경로 없음\n\n");

    fclose(f);
    printf("\n  [결과가 data/result.txt에 저장되었습니다]\n");
}

int main(void) {
    Graph*        g = graph_create();
    StationHash*  h = hash_create();
    HistoryStack* s = history_create();

    ui_clear();
    ui_print_title();
    printf("  데이터 로드 중...\n");

    int loaded = load_subway_data(g, h);
    if (!loaded) {
        printf("  CSV 없음. 기본 데이터로 실행합니다.\n");
        load_default_data(g, h);
    } else {
        printf("  %d개 역 로드 완료!\n", graph_station_count(g));
    }

    printf("\n  아무 키나 누르면 시작합니다...\n");
    getchar();

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
            printf("  * 전체 이름을 정확히 입력해주세요\n\n");
            printf("출발역 입력: ");
            scanf("%63s", from_name);
            printf("도착역 입력: ");
            scanf("%63s", to_name);

            remove_station_suffix(from_name);
            remove_station_suffix(to_name);

            int from_id = hash_find(h, from_name);
            int to_id   = hash_find(h, to_name);

            if (from_id == -1) {
                printf("\n[!] '%s' 역을 찾을 수 없습니다.\n", from_name);
                suggest_station(g, from_name);
                printf("\n아무 키나 누르세요...\n"); getchar(); getchar();
                continue;
            }
            if (to_id == -1) {
                printf("\n[!] '%s' 역을 찾을 수 없습니다.\n", to_name);
                suggest_station(g, to_name);
                printf("\n아무 키나 누르세요...\n"); getchar(); getchar();
                continue;
            }
            if (from_id == to_id) {
                printf("\n[!] 출발역과 도착역이 같습니다.\n");
                printf("아무 키나 누르세요...\n"); getchar(); getchar();
                continue;
            }

            /* A→B 경로 탐색 */
            PathResult dijk_ab = dijkstra(g, from_id, to_id);
            PathResult bfs_ab  = bfs_min_transfer(g, from_id, to_id);

            /* B→A 경로 탐색 */
            PathResult dijk_ba = dijkstra(g, to_id, from_id);
            PathResult bfs_ba  = bfs_min_transfer(g, to_id, from_id);

            ui_print_result(g, &dijk_ab, &bfs_ab, from_name, to_name);
            printf("\n");
            ui_print_result(g, &dijk_ba, &bfs_ba, to_name, from_name);

            if (dijk_ab.found)
                history_push(s, from_name, to_name,
                             dijk_ab.total_time, dijk_ab.transfers);

            printf("\n  결과를 파일로 저장하시겠습니까? (1=예 / 기타=아니오): ");
            int save_choice;
            if (scanf("%d", &save_choice) == 1 && save_choice == 1)
                save_result_to_file(g, &dijk_ab, &bfs_ab, from_name, to_name);

            printf("\n아무 키나 누르세요...\n"); getchar(); getchar();

        } else if (choice == 2) {
            ui_clear();
            ui_print_title();
            history_print(s);
            printf("\n아무 키나 누르세요...\n"); getchar(); getchar();

        } else if (choice == 3) {
            ui_clear();
            ui_print_title();
            printf("=== 전체 역 목록 ===\n\n");
            for (int line = 1; line <= 9; line++) {
                printf("[%d호선]\n", line);
                int cnt = 0;
                for (int i = 0; i < MAX_STATIONS; i++) {
                    if (!g->stations[i].valid) continue;
                    if (g->stations[i].line != line) continue;
                    printf("  %-20s", g->stations[i].name);
                    cnt++;
                    if (cnt % 3 == 0) printf("\n");
                }
                printf("\n\n");
            }
            printf("아무 키나 누르세요...\n"); getchar(); getchar();

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
