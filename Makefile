CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
SRCS   = src/main.c src/graph.c src/dijkstra.c src/bfs.c \
         src/station.c src/history.c src/ui.c src/file_io.c
TARGET = subway

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
