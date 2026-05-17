# 서울 지하철 경로 탐색기

## 개발 환경
- Language: C (C11)
- OS: Windows / Linux
- Compiler: GCC

## 프로젝트 구조
```
subway_pathfinder/
├── src/
│   ├── main.c        # 메인 루프, 메뉴
│   ├── graph.c       # 그래프 (인접 리스트)       ← 팀원 A
│   ├── dijkstra.c    # 다익스트라 + 최소 힙       ← 팀원 A
│   ├── bfs.c         # BFS 최소 환승              ← 팀원 A
│   ├── station.c     # 해시 테이블                ← 팀원 B
│   ├── history.c     # 검색 기록 스택             ← 팀원 B
│   ├── ui.c          # 메뉴 + 결과 출력           ← 팀원 B
│   └── file_io.c     # CSV 파일 입출력            ← 팀원 B
├── include/          # 헤더 파일
├── data/
│   └── subway.csv    # 노선 데이터 (선택)
└── README.md
```

## 빌드 및 실행

### Windows
```
gcc -Wall -std=c11 -Iinclude -o subway src/main.c src/graph.c src/dijkstra.c src/bfs.c src/station.c src/history.c src/ui.c src/file_io.c
subway.exe
```

### Linux
```
make
./subway
```

## 사용법
1. 프로그램 실행
2. 메뉴에서 1번 선택
3. 출발역, 도착역 입력 (예: 시청, 왕십리)
4. 최단 시간 경로(다익스트라)와 최소 환승 경로(BFS) 동시 출력

## 사용 자료구조
| 자료구조 | 파일 | 용도 |
|---|---|---|
| 그래프 (인접 리스트) | graph.c | 노선 연결 |
| 우선순위 큐 (최소 힙) | dijkstra.c | 다익스트라 |
| 큐 (BFS) | bfs.c | 최소 환승 |
| 해시 테이블 | station.c | 역 이름 검색 |
| 스택 | history.c | 검색 기록 |
