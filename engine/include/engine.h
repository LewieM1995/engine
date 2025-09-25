#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include "tile_size.c"
#include "levels.h"

#define MAX_TEXTURES 100

typedef struct {
    float x, y;  // position
    float rotation; // in degrees
    float scale;
    int sprite_id;
    int layer; // for layering
} RenderCommand;

typedef struct {
    float x, y;
    float target_x, target_y;
    float smoothing;
    int screen_width, screen_height;
} Camera;

void engine_init(SDL_Renderer *renderer);
void engine_begin_frame();
void engine_submit(RenderCommand cmd);
void engine_end_frame();
void engine_shutdown();

int engine_load_texture(const char* filepath);
void engine_unload_all_textures();

void camera_init(Camera* camera, int screen_width, int screen_height);
void camera_follow(Camera* camera, float target_x, float target_y);
void camera_update(Camera *cam, float steptime, Map *map, float player_x, float player_y);
void camera_screen_to_world(Camera *cam, float screen_x, float screen_y, float *world_x, float *world_y);
void camera_world_to_screen(Camera *cam, float world_x, float world_y, float *screen_x, float *screen_y);

#endif
