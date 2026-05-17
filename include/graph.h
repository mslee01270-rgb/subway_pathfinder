#ifndef GRAPH_H
#define GRAPH_H

#define MAX_STATIONS 300
#define MAX_NAME_LEN 64
#define INF 99999999

/* 간선 노드 (인접 리스트) */
typedef struct EdgeNode {
    int to;              /* 연결된 역 ID */
    int time;            /* 소요 시간 (분) */
    int line;            /* 노선 번호 */
    struct EdgeNode* next;
} EdgeNode;

/* 역 정보 */
typedef struct {
    int  id;
    char name[MAX_NAME_LEN];
    int  line;           /* 대표 노선 */
    int  valid;          /* 사용 여부 */
} Station;

/* 그래프 */
typedef struct {
    Station  stations[MAX_STATIONS];
    EdgeNode* adj[MAX_STATIONS];
    int      station_count;
} Graph;

/* 함수 선언 */
Graph*  graph_create(void);
void    graph_destroy(Graph* g);
int     graph_add_station(Graph* g, int id, const char* name, int line);
void    graph_add_edge(Graph* g, int from, int to, int time, int line);
void    graph_print(Graph* g);
int     graph_station_count(Graph* g);

#endif /* GRAPH_H */
