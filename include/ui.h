#ifndef UI_H
#define UI_H

#include "graph.h"
#include "dijkstra.h"
#include "history.h"

void ui_clear(void);
void ui_print_title(void);
void ui_print_menu(void);
void ui_print_result(Graph* g, PathResult* dijk, PathResult* bfs,
                     const char* from, const char* to);

#endif /* UI_H */
