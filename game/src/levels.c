#include "levels.h"
#include <string.h>
#include <time.h>
#include "engine.h"

float perlin_noise(float x, float y, uint32_t seed)
{
    int n = (int)x + (int)y * 57 + seed;
    n = (n << 13) ^ n;
    return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

void generate_map(Map *map, uint32_t seed)
{
    srand(seed); // Seed the random number generator

    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++)
        {
            Cell *cell = get_cell(map, x, y);

            float noise = perlin_noise(x * 0.1, y * 0.1, seed);

            if (noise > 0.3f)
            {
                cell->flags = 0; // Not walkable, not transparent
                cell->tile_type = TILE_WALL;
            }
            else
            {
                cell->flags = WALKABLE | TRANSPARENT;
                cell->tile_type = TILE_FLOOR;
            }
        }
    }
}

Map *create_map(int width, int height)
{
    Map *map = (Map *)malloc(sizeof(Map));
    map->width = width;
    map->height = height;
    map->cells = (Cell *)malloc(sizeof(Cell) * width * height);
    return map;
}

void cleanup_map(Map *map)
{
    if (map)
    {
        free(map->cells);
        free(map);
    }
}

LevelConfig load_level_config(int level_number) {
    LevelConfig config = {0};
    char filename[256];
    snprintf(filename, sizeof(filename), "levels/configs/level_%03d.cfg", level_number);
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        // Default config if file doesn't exist
        config.seed = level_number * 1337;
        config.width = 50;
        config.height = 50;
        config.difficulty = 1;
        config.water_chance = 0.1;
        return config;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "seed=", 5) == 0) {
            config.seed = atoi(line + 5);
        } else if (strncmp(line, "width=", 6) == 0) {
            config.width = atoi(line + 6);
        } else if (strncmp(line, "height=", 7) == 0) {
            config.height = atoi(line + 7);
        }
        // Add more parameter parsing as needed
    }
    
    fclose(file);
    return config;
}