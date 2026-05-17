#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "station.h"

/* djb2 해시 함수 */
static unsigned int hash_func(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

StationHash* hash_create(void) {
    StationHash* h = (StationHash*)calloc(1, sizeof(StationHash));
    return h;
}

void hash_destroy(StationHash* h) {
    if (!h) return;
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry* cur = h->buckets[i];
        while (cur) {
            HashEntry* nxt = cur->next;
            free(cur);
            cur = nxt;
        }
    }
    free(h);
}

void hash_insert(StationHash* h, const char* name, int id) {
    unsigned int idx = hash_func(name);
    /* 중복 확인 */
    HashEntry* cur = h->buckets[idx];
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            cur->id = id;  /* 덮어쓰기 */
            return;
        }
        cur = cur->next;
    }
    /* 새 항목 삽입 */
    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry));
    strncpy(entry->name, name, MAX_NAME_LEN - 1);
    entry->id   = id;
    entry->next = h->buckets[idx];
    h->buckets[idx] = entry;
}

int hash_find(StationHash* h, const char* name) {
    unsigned int idx = hash_func(name);
    HashEntry* cur = h->buckets[idx];
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur->id;
        cur = cur->next;
    }
    return -1;
}

void hash_build_from_graph(StationHash* h, Graph* g) {
    for (int i = 0; i < MAX_STATIONS; i++) {
        if (g->stations[i].valid)
            hash_insert(h, g->stations[i].name, i);
    }
}
