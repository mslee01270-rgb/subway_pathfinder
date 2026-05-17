#include <stdio.h>
#include <stdlib.h>
#include "ui.h"

void ui_clear(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ui_print_title(void) {
    printf("╔══════════════════════════════════════╗\n");
    printf("║      서울 지하철 경로 탐색기 v1.0     ║\n");
    printf("║   그래프 + 다익스트라 + BFS 구현      ║\n");
    printf("╚══════════════════════════════════════╝\n\n");
}

void ui_print_menu(void) {
    printf("  1) 경로 탐색 (출발역 → 도착역)\n");
    printf("  2) 최근 검색 기록 보기\n");
    printf("  3) 전체 역 목록 보기\n");
    printf("  4) 종료\n");
    printf("\n선택 > ");
}

void ui_print_result(Graph* g, PathResult* dijk, PathResult* bfs,
                     const char* from, const char* to) {
    printf("\n┌──────────────────────────────────────┐\n");
    printf("│  %s → %s\n", from, to);
    printf("├──────────────────────────────────────┤\n");

    printf("│ ★ 최단 시간 경로 (다익스트라)\n");
    if (dijk->found) {
        printf("│  ");
        for (int i = 0; i < dijk->path_len; i++) {
            printf("%s", g->stations[dijk->path[i]].name);
            if (i < dijk->path_len - 1) printf(" → ");
        }
        printf("\n│  총 %d분  |  환승 %d회\n", dijk->total_time, dijk->transfers);
    } else {
        printf("│  경로 없음\n");
    }

    printf("├──────────────────────────────────────┤\n");

    printf("│ ★ 최소 환승 경로 (BFS)\n");
    if (bfs->found) {
        printf("│  ");
        for (int i = 0; i < bfs->path_len; i++) {
            printf("%s", g->stations[bfs->path[i]].name);
            if (i < bfs->path_len - 1) printf(" → ");
        }
        printf("\n│  총 %d분  |  환승 %d회\n", bfs->total_time, bfs->transfers);
    } else {
        printf("│  경로 없음\n");
    }

    printf("└──────────────────────────────────────┘\n");
}
