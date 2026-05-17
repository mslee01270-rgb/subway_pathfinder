#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_MAX 20
#define NAME_LEN    64

typedef struct {
    char from[NAME_LEN];
    char to[NAME_LEN];
    int  total_time;
    int  transfers;
} HistoryEntry;

typedef struct {
    HistoryEntry data[HISTORY_MAX];
    int          top;
} HistoryStack;

/* 함수 선언 */
HistoryStack* history_create(void);
void          history_destroy(HistoryStack* s);
void          history_push(HistoryStack* s, const char* from, const char* to, int time, int transfers);
int           history_pop(HistoryStack* s, HistoryEntry* out);
void          history_print(HistoryStack* s);
int           history_empty(HistoryStack* s);

#endif /* HISTORY_H */
