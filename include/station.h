#ifndef STATION_H
#define STATION_H

#include "graph.h"

#define HASH_SIZE 512

/* 해시 테이블 버킷 */
typedef struct HashEntry {
    char            name[MAX_NAME_LEN];
    int             id;
    struct HashEntry* next;
} HashEntry;

/* 해시 테이블 */
typedef struct {
    HashEntry* buckets[HASH_SIZE];
} StationHash;

/* 함수 선언 */
StationHash* hash_create(void);
void         hash_destroy(StationHash* h);
void         hash_insert(StationHash* h, const char* name, int id);
int          hash_find(StationHash* h, const char* name);   /* 없으면 -1 */
void         hash_build_from_graph(StationHash* h, Graph* g);

#endif /* STATION_H */
