#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef LEVELS_H
#define LEVELS_H

#define WALKABLE 0x01
#define TRANSPARENT 0x02
#define DOOR 0x04
#define WALL 0x08
#define FLOOR 0x10

#define TILE_FLOOR 1
#define TILE_WALL 2  
#define TILE_DOOR 3
#define TILE_WATER 4

typedef struct {
    uint8_t flags;
    uint8_t tile_type;
} Cell;

typedef struct {
    Cell *cells;
    int width;
    int height;
} Map;

typedef struct {
    uint32_t seed;
    int width, height;
    int difficulty;
    float water_chance;
} LevelConfig;

Map* create_map(int width, int height);
void generate_map(Map* map, uint32_t seed);
void cleanup_map(Map* map);
LevelConfig load_level_config(int level_number);

static inline Cell* get_cell(Map* map, int x, int y) {
    return &map->cells[y * map->width + x];
}

static inline int is_walkable(Cell* cell) {
    return cell->flags & WALKABLE;
}

#endif // LEVELS_H