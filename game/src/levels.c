#include "levels.h"
#include <time.h>
#include "engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void connect_floor_areas(Map *map, int start_x, int start_y);
void flood_fill(Map *map, int *visited, int x, int y, int mark);
void create_path_to_main_area(Map *map, int *visited, int start_x, int start_y);
void add_decorative_features(Map *map, uint32_t seed);

// Cellular automata map generation with guaranteed connectivity
void generate_map(Map *map, uint32_t seed)
{
    srand(seed);
    
    // Initialize with random noise (35% walls)
    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++)
        {
            Cell *cell = get_cell(map, x, y);
            
            // Force border to be walls
            if (x == 0 || x == map->width - 1 || y == 0 || y == map->height - 1)
            {
                cell->flags = 0;
                cell->tile_type = TILE_WALL;
            }
            else
            {
                // 35% chance of wall in interior
                if (rand() % 100 < 35)
                {
                    cell->flags = 0;
                    cell->tile_type = TILE_WALL;
                }
                else
                {
                    cell->flags = WALKABLE;
                    cell->tile_type = TILE_FLOOR;
                }
            }
        }
    }

    // Cellular automata smoothing (3 iterations)
    for (int iteration = 0; iteration < 3; iteration++)
    {
        // Create temporary map for new state
        Cell *temp_cells = malloc(sizeof(Cell) * map->width * map->height);
        memcpy(temp_cells, map->cells, sizeof(Cell) * map->width * map->height);
        
        for (int y = 1; y < map->height - 1; y++)
        {
            for (int x = 1; x < map->width - 1; x++)
            {
                int wall_count = 0;
                
                // Count walls in 3x3 neighborhood
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        Cell *neighbor = get_cell(map, x + dx, y + dy);
                        if (neighbor->tile_type == TILE_WALL)
                            wall_count++;
                    }
                }
                
                Cell *temp_cell = &temp_cells[y * map->width + x];
                
                // Apply cellular automata rule
                if (wall_count >= 5)
                {
                    temp_cell->flags = 0;
                    temp_cell->tile_type = TILE_WALL;
                }
                else
                {
                    temp_cell->flags = WALKABLE;
                    temp_cell->tile_type = TILE_FLOOR;
                }
            }
        }
        
        // Copy temp back to main map
        memcpy(map->cells, temp_cells, sizeof(Cell) * map->width * map->height);
        free(temp_cells);
    }

    // Spawn area is clear (center of map)
    int center_x = map->width / 2;
    int center_y = map->height / 2;
    int clear_radius = 3;
    
    for (int dy = -clear_radius; dy <= clear_radius; dy++)
    {
        for (int dx = -clear_radius; dx <= clear_radius; dx++)
        {
            int px = center_x + dx;
            int py = center_y + dy;
            
            if (px >= 0 && px < map->width && py >= 0 && py < map->height)
            {
                Cell *cell = get_cell(map, px, py);
                cell->flags = WALKABLE;
                cell->tile_type = TILE_FLOOR;
            }
        }
    }

    // Connect isolated floor areas using flood fill
    connect_floor_areas(map, center_x, center_y);

    // add_decorative_features(map, seed);
}

void connect_floor_areas(Map *map, int start_x, int start_y)
{
    // Create visited array
    int *visited = calloc(map->width * map->height, sizeof(int));
    
    // Flood fill from spawn point to mark main area
    flood_fill(map, visited, start_x, start_y, 1);
    
    // Find unconnected floor tiles and create paths to them
    for (int y = 1; y < map->height - 1; y++)
    {
        for (int x = 1; x < map->width - 1; x++)
        {
            Cell *cell = get_cell(map, x, y);
            int index = y * map->width + x;
            
            // If floor tile but not connected to main area
            if (cell->tile_type == TILE_FLOOR && !visited[index])
            {
                // Create a path from isolated area to the main area
                create_path_to_main_area(map, visited, x, y);
            }
        }
    }
    
    free(visited);
}

void flood_fill(Map *map, int *visited, int x, int y, int mark)
{
    if (x < 0 || x >= map->width || y < 0 || y >= map->height)
        return;
    
    int index = y * map->width + x;
    if (visited[index])
        return;
    
    Cell *cell = get_cell(map, x, y);
    if (cell->tile_type != TILE_FLOOR)
        return;
    
    visited[index] = mark;
    
    // Recursively fill adjacent cells
    flood_fill(map, visited, x + 1, y, mark);
    flood_fill(map, visited, x - 1, y, mark);
    flood_fill(map, visited, x, y + 1, mark);
    flood_fill(map, visited, x, y - 1, mark);
}

void create_path_to_main_area(Map *map, int *visited, int start_x, int start_y)
{
    // Simple pathfinding: move towards center while clearing walls
    int target_x = map->width / 2;
    int target_y = map->height / 2;
    
    int current_x = start_x;
    int current_y = start_y;
    
    // Create path towards center
    while (current_x != target_x || current_y != target_y)
    {
        // Move towards target
        if (current_x < target_x) current_x++;
        else if (current_x > target_x) current_x--;
        
        if (current_y < target_y) current_y++;
        else if (current_y > target_y) current_y--;
        
        // Clear this cell and mark as connected
        Cell *cell = get_cell(map, current_x, current_y);
        cell->flags = WALKABLE;
        cell->tile_type = TILE_FLOOR;
        visited[current_y * map->width + current_x] = 1;
        
        // If reached the main area, stop
        if (visited[current_y * map->width + current_x])
            break;
    }
}

void add_decorative_features(Map *map, uint32_t seed)
{
    srand(seed + 12345); // Different seed for decorations
    
    // Add some water patches in open areas
    for (int attempts = 0; attempts < map->width * map->height / 50; attempts++)
    {
        int x = 2 + rand() % (map->width - 4);
        int y = 2 + rand() % (map->height - 4);
        
        // Check if area is clear (3x3)
        int clear = 1;
        for (int dy = -1; dy <= 1 && clear; dy++)
        {
            for (int dx = -1; dx <= 1 && clear; dx++)
            {
                Cell *cell = get_cell(map, x + dx, y + dy);
                if (cell->tile_type != TILE_FLOOR)
                    clear = 0;
            }
        }
        
        // Place small water patch if area is clear and not near spawn
        int center_x = map->width / 2;
        int center_y = map->height / 2;
        int dist_to_center = abs(x - center_x) + abs(y - center_y);
        
        if (clear && dist_to_center > 10)
        {
            Cell *cell = get_cell(map, x, y);
            cell->flags = 0; // Water is not walkable
            cell->tile_type = TILE_WATER;
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
    snprintf(filename, sizeof(filename), "game/levels/configs/level_%03d.cfg", level_number);
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        // Default config if file doesn't exist
        config.seed = level_number * 21323;
        config.width = 100;
        config.height = 100;
        config.difficulty = 1;
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