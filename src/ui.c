#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"

/*
 * ui.c - 화면 출력 및 메뉴 구현
 *
 * ANSI 색상 코드로 노선별 색상을 구분한다.
 * 1호선: 파랑, 2호선: 초록, 3호선: 주황, 4호선: 하늘
 * 5호선: 보라, 6호선: 갈색, 7호선: 올리브, 8호선: 분홍, 9호선: 금색
 */

/* ANSI 색상 코드 */
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define ORANGE  "\033[38;5;208m"
#define BROWN   "\033[38;5;130m"
#define OLIVE   "\033[38;5;100m"
#define PINK    "\033[38;5;205m"
#define GOLD    "\033[38;5;220m"

/* 노선 번호에 따른 색상 반환 */
static const char* line_color(int line) {
    switch (line) {
        case 1: return BLUE;
        case 2: return GREEN;
        case 3: return ORANGE;
        case 4: return CYAN;
        case 5: return MAGENTA;
        case 6: return BROWN;
        case 7: return OLIVE;
        case 8: return PINK;
        case 9: return GOLD;
        default: return WHITE;
    }
}

void ui_clear(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ui_print_title(void) {
    printf(BOLD CYAN);
    printf("╔══════════════════════════════════════╗\n");
    printf("║      서울 지하철 경로 탐색기 v1.0     ║\n");
    printf("║   그래프 + 다익스트라 + BFS 구현      ║\n");
    printf("╚══════════════════════════════════════╝\n\n");
    printf(RESET);
}

void ui_print_menu(void) {
    printf("  1) 경로 탐색 (출발역 → 도착역)\n");
    printf("  2) 최근 검색 기록 보기\n");
    printf("  3) 전체 역 목록 보기\n");
    printf("  4) 종료\n");
    printf("\n선택 > ");
}

/*
 * 경로 출력 - 노선 정보 + 환승 표시 포함
 * 예: 강남 →(2호선)→ 교대 →[3호선 환승]→ 고속터미널
 */
static void print_path_detail(Graph* g, PathResult* r) {
    if (!r->found || r->path_len == 0) {
        printf("  경로를 찾을 수 없습니다.\n");
        return;
    }

    printf("  ");
    for (int i = 0; i < r->path_len; i++) {
        int id      = r->path[i];
        int line    = g->stations[id].line;
        const char* col = line_color(line);

        /* 역 이름 출력 */
        printf("%s%s%s", col, g->stations[id].name, RESET);

        if (i < r->path_len - 1) {
            int next_id   = r->path[i+1];
            int next_line = g->stations[next_id].line;

            if (next_line != line) {
                /* 환승 */
                printf(" %s→[%d호선 환승]→%s ", YELLOW, next_line, RESET);
            } else {
                /* 같은 노선 */
                printf("%s →%s ", col, RESET);
            }
        }
    }
    printf("\n");
}

void ui_print_result(Graph* g, PathResult* dijk, PathResult* bfs,
                     const char* from, const char* to) {
    printf("\n");
    printf(BOLD "┌──────────────────────────────────────────┐\n" RESET);
    printf(BOLD "│  %s → %s\n" RESET, from, to);
    printf(BOLD "├──────────────────────────────────────────┤\n" RESET);

    /* 최단 시간 경로 (다익스트라) */
    printf(BOLD GREEN "│ ★ 최단 시간 경로 (다익스트라)\n" RESET);
    if (dijk->found) {
        print_path_detail(g, dijk);
        printf("  " BOLD "총 %d분" RESET "  |  환승 %d회\n",
               dijk->total_time, dijk->transfers);
    } else {
        printf("  경로를 찾을 수 없습니다.\n");
    }

    printf(BOLD "├──────────────────────────────────────────┤\n" RESET);

    /* 최소 환승 경로 (BFS) */
    printf(BOLD CYAN "│ ★ 최소 환승 경로 (BFS)\n" RESET);
    if (bfs->found) {
        print_path_detail(g, bfs);
        printf("  " BOLD "총 %d분" RESET "  |  환승 %d회\n",
               bfs->total_time, bfs->transfers);
    } else {
        printf("  경로를 찾을 수 없습니다.\n");
    }

    printf(BOLD "└──────────────────────────────────────────┘\n" RESET);
}
