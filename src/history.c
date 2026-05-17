#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

HistoryStack* history_create(void) {
    HistoryStack* s = (HistoryStack*)malloc(sizeof(HistoryStack));
    s->top = -1;
    return s;
}

void history_destroy(HistoryStack* s) { free(s); }

void history_push(HistoryStack* s, const char* from, const char* to,
                  int time, int transfers) {
    if (s->top >= HISTORY_MAX - 1) {
        /* 꽉 차면 맨 아래 제거 후 위로 밀기 */
        for (int i = 0; i < HISTORY_MAX - 1; i++)
            s->data[i] = s->data[i + 1];
        s->top = HISTORY_MAX - 2;
    }
    s->top++;
    strncpy(s->data[s->top].from, from, NAME_LEN - 1);
    strncpy(s->data[s->top].to,   to,   NAME_LEN - 1);
    s->data[s->top].total_time = time;
    s->data[s->top].transfers  = transfers;
}

int history_pop(HistoryStack* s, HistoryEntry* out) {
    if (s->top < 0) return 0;
    *out = s->data[s->top--];
    return 1;
}

void history_print(HistoryStack* s) {
    if (s->top < 0) {
        printf("  검색 기록이 없습니다.\n");
        return;
    }
    printf("\n=== 최근 검색 기록 ===\n");
    for (int i = s->top; i >= 0; i--) {
        printf("  [%2d] %s → %s  (%d분, 환승 %d회)\n",
               s->top - i + 1,
               s->data[i].from, s->data[i].to,
               s->data[i].total_time, s->data[i].transfers);
    }
}

int history_empty(HistoryStack* s) { return s->top < 0; }
