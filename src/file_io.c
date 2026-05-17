#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_io.h"

/*
 * subway.csv 형식:
 * from_id,from_name,to_id,to_name,line,time
 * 예: 1,서울역,2,시청,1,2
 */
int load_subway_data(Graph* g, StationHash* h) {
    FILE* f = fopen(SUBWAY_CSV, "r");
    if (!f) return 0;

    char line[256];
    fgets(line, sizeof(line), f); /* 헤더 스킵 */

    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        int  from_id, to_id, lineno, time;
        char from_name[MAX_NAME_LEN], to_name[MAX_NAME_LEN];

        if (sscanf(line, "%d,%63[^,],%d,%63[^,],%d,%d",
                   &from_id, from_name, &to_id, to_name, &lineno, &time) != 6)
            continue;

        graph_add_station(g, from_id, from_name, lineno);
        graph_add_station(g, to_id,   to_name,   lineno);
        graph_add_edge(g, from_id, to_id, time, lineno);
        hash_insert(h, from_name, from_id);
        hash_insert(h, to_name,   to_id);
        count++;
    }
    fclose(f);
    printf("노선 데이터 로드 완료: %d개 구간\n", count);
    return count;
}

/* CSV 없을 때 사용하는 서울 지하철 기본 데이터 (1~5호선 주요 역) */
void load_default_data(Graph* g, StationHash* h) {

    /* ── 1호선 ── */
    int line1[][3] = {
        {1,2,2},{2,3,2},{3,4,2},{4,5,3},{5,6,2},
        {6,7,2},{7,8,3},{8,9,2},{9,10,2}
    };
    const char* name1[] = {
        "소요산","동두천","보산","동두천중앙","지행",
        "덕정","덕계","양주","녹양","가능"
    };
    for (int i = 0; i < 10; i++)
        graph_add_station(g, i+1, name1[i], 1);
    for (int i = 0; i < 9; i++)
        graph_add_edge(g, line1[i][0], line1[i][1], line1[i][2], 1);

    /* ── 2호선 (순환) ── */
    int s2[] = {101,102,103,104,105,106,107,108,109,110,
                111,112,113,114,115,116,117,118,119,120};
    const char* name2[] = {
        "시청","을지로입구","을지로3가","을지로4가","동대문역사문화공원",
        "신당","상왕십리","왕십리","한양대","뚝섬",
        "성수","건대입구","구의","강변","잠실나루",
        "잠실","신천","종합운동장","삼성","선릉"
    };
    for (int i = 0; i < 20; i++)
        graph_add_station(g, s2[i], name2[i], 2);
    for (int i = 0; i < 19; i++)
        graph_add_edge(g, s2[i], s2[i+1], 2, 2);
    graph_add_edge(g, s2[19], s2[0], 2, 2); /* 순환 */

    /* ── 3호선 ── */
    int s3[] = {201,202,203,204,205,206,207,208,209,210};
    const char* name3[] = {
        "대화","주엽","정발산","마두","백석",
        "대곡","화정","원당","원흥","삼송"
    };
    for (int i = 0; i < 10; i++)
        graph_add_station(g, s3[i], name3[i], 3);
    for (int i = 0; i < 9; i++)
        graph_add_edge(g, s3[i], s3[i+1], 3, 3);

    /* ── 4호선 ── */
    int s4[] = {301,302,303,304,305,306,307,308,309,310};
    const char* name4[] = {
        "당고개","상계","노원","창동","쌍문",
        "수유","미아","미아사거리","길음","성신여대입구"
    };
    for (int i = 0; i < 10; i++)
        graph_add_station(g, s4[i], name4[i], 4);
    for (int i = 0; i < 9; i++)
        graph_add_edge(g, s4[i], s4[i+1], 2, 4);

    /* ── 5호선 ── */
    int s5[] = {401,402,403,404,405,406,407,408,409,410};
    const char* name5[] = {
        "방화","개화산","김포공항","송정","마곡",
        "발산","우장산","화곡","까치산","신정"
    };
    for (int i = 0; i < 10; i++)
        graph_add_station(g, s5[i], name5[i], 5);
    for (int i = 0; i < 9; i++)
        graph_add_edge(g, s5[i], s5[i+1], 2, 5);

    /* ── 주요 환승역 연결 ── */
    /* 시청: 1호선(3) ↔ 2호선(101) */
    graph_add_edge(g, 3, 101, 0, 1);
    /* 왕십리: 2호선(108) ↔ 5호선(내부 연결 생략, 실제론 있음) */

    /* 해시 테이블 구축 */
    hash_build_from_graph(h, g);

    printf("기본 데이터 로드 완료 (%d개 역)\n", graph_station_count(g));
}
