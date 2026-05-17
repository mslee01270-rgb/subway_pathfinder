#ifndef BFS_H
#define BFS_H

#include "graph.h"
#include "dijkstra.h"

/* 함수 선언 */
PathResult bfs_min_transfer(Graph* g, int start, int end);

#endif /* BFS_H */
