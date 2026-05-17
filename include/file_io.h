#ifndef FILE_IO_H
#define FILE_IO_H

#include "graph.h"
#include "station.h"

#define SUBWAY_CSV "data/subway.csv"

int load_subway_data(Graph* g, StationHash* h);
void load_default_data(Graph* g, StationHash* h);  /* CSV 없을 때 기본 데이터 */

#endif /* FILE_IO_H */
