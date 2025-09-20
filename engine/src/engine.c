#include "engine.h"
#include "levels.h"
#include <SDL2/SDL_image.h>
#include <math.h>

static SDL_Renderer *sdl_renderer = NULL;
static SDL_Texture *textures[MAX_TEXTURES];
static int texture_count = 0; // Simple texture array

void engine_init(SDL_Renderer *renderer)
{
    sdl_renderer = renderer;

    // Initialize IMG for PNG loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    }

    // Initialize texture array
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        textures[i] = NULL;
    }
    texture_count = 0;

    // Load your game textures
    engine_load_texture("engine/assets/player.png"); // ID 0 (Player)
    engine_load_texture("engine/assets/floor.png");  // ID 1 (TILE_FLOOR)
    engine_load_texture("engine/assets/wall.png");   // ID 2 (TILE_WALL)
    engine_load_texture("engine/assets/exit.png");   // ID 3 (TILE_DOOR)
    engine_load_texture("engine/assets/water.png");  // ID 4 (TILE_WATER)
}

int engine_load_texture(const char *filepath)
{
    if (texture_count >= MAX_TEXTURES)
    {
        printf("Cannot load more textures, max reached!\n");
        return -1;
    }

    SDL_Surface *surface = IMG_Load(filepath);
    if (!surface)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", filepath, IMG_GetError());
        return -1;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        printf("Unable to create texture from %s! SDL Error: %s\n", filepath, SDL_GetError());
        return -1;
    }

    textures[texture_count] = texture;
    printf("Loaded texture %s as ID %d\n", filepath, texture_count);
    return texture_count++;
}

void engine_unload_all_textures()
{
    for (int i = 0; i < texture_count; i++)
    {
        if (textures[i])
        {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }
    texture_count = 0;
}

void engine_begin_frame()
{
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(sdl_renderer);
}

void engine_submit(RenderCommand cmd)
{
    // Check if we have a texture for this sprite_id
    if (cmd.sprite_id >= 0 && cmd.sprite_id < texture_count && textures[cmd.sprite_id])
    {
        // Render using texture
        SDL_Texture *texture = textures[cmd.sprite_id];

        // Get texture dimensions
        int tex_width, tex_height;
        SDL_QueryTexture(texture, NULL, NULL, &tex_width, &tex_height);

        // Calculate scaled size
        int scaled_width = (int)(tex_width * cmd.scale);
        int scaled_height = (int)(tex_height * cmd.scale);

        // Destination rectangle (where to draw on screen)
        SDL_Rect dst_rect = {
            (int)(cmd.x - scaled_width / 2),
            (int)(cmd.y - scaled_height / 2),
            scaled_width,
            scaled_height};

        // Render with rotation if needed
        if (cmd.rotation != 0.0f)
        {
            SDL_RenderCopyEx(sdl_renderer, texture, NULL, &dst_rect, cmd.rotation, NULL, SDL_FLIP_NONE);
        }
        else
        {
            SDL_RenderCopy(sdl_renderer, texture, NULL, &dst_rect);
        }
    }
    else
    {
        // Fallback to colored rectangles if texture not found
        int half_size = (int)(16 * cmd.scale);
        float angleRad = cmd.rotation * (3.14159f / 180.0f);

        // Different rendering based on sprite_id
        switch (cmd.sprite_id)
        {
        case 0: // Player
        {
            // Your existing player rendering code
            float corners[4][2] = {
                {-half_size, -half_size}, {half_size, -half_size}, {half_size, half_size}, {-half_size, half_size}};

            SDL_Point square_points[5];
            for (int i = 0; i < 4; i++)
            {
                float rotated_x = corners[i][0] * cosf(angleRad) - corners[i][1] * sinf(angleRad);
                float rotated_y = corners[i][0] * sinf(angleRad) + corners[i][1] * cosf(angleRad);

                square_points[i].x = (int)(cmd.x + rotated_x);
                square_points[i].y = (int)(cmd.y + rotated_y);
            }
            square_points[4] = square_points[0];

            SDL_SetRenderDrawColor(sdl_renderer, 100, 100, 100, 255);
            SDL_RenderDrawLines(sdl_renderer, square_points, 5);

            // Triangle
            float tri_size = half_size * 0.8f;
            SDL_Point p1 = {(int)(cmd.x + cosf(angleRad) * tri_size), (int)(cmd.y + sinf(angleRad) * tri_size)};
            SDL_Point p2 = {(int)(cmd.x + cosf(angleRad + 2.618f) * (tri_size * 0.6f)), (int)(cmd.y + sinf(angleRad + 2.618f) * (tri_size * 0.6f))};
            SDL_Point p3 = {(int)(cmd.x + cosf(angleRad - 2.618f) * (tri_size * 0.6f)), (int)(cmd.y + sinf(angleRad - 2.618f) * (tri_size * 0.6f))};

            SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
            SDL_RenderDrawLine(sdl_renderer, p1.x, p1.y, p2.x, p2.y);
            SDL_RenderDrawLine(sdl_renderer, p2.x, p2.y, p3.x, p3.y);
            SDL_RenderDrawLine(sdl_renderer, p3.x, p3.y, p1.x, p1.y);
            break;
        }

        case TILE_FLOOR: // Floor tiles (1)
        {
            SDL_Rect floor_rect = {(int)(cmd.x - half_size), (int)(cmd.y - half_size), half_size * 2, half_size * 2};
            SDL_SetRenderDrawColor(sdl_renderer, 139, 69, 19, 255); // Brown
            SDL_RenderFillRect(sdl_renderer, &floor_rect);
            break;
        }

        case TILE_WALL: // Wall tiles (2)
        {
            SDL_Rect wall_rect = {(int)(cmd.x - half_size), (int)(cmd.y - half_size), half_size * 2, half_size * 2};
            SDL_SetRenderDrawColor(sdl_renderer, 64, 64, 64, 255); // Dark gray
            SDL_RenderFillRect(sdl_renderer, &wall_rect);
            SDL_SetRenderDrawColor(sdl_renderer, 32, 32, 32, 255); // Darker border
            SDL_RenderDrawRect(sdl_renderer, &wall_rect);
            break;
        }

        case TILE_DOOR: // Door tiles (3)
        {
            SDL_Rect door_rect = {(int)(cmd.x - half_size), (int)(cmd.y - half_size), half_size * 2, half_size * 2};
            SDL_SetRenderDrawColor(sdl_renderer, 101, 67, 33, 255); // Dark brown
            SDL_RenderFillRect(sdl_renderer, &door_rect);

            SDL_Rect handle = {(int)(cmd.x + half_size / 2), (int)(cmd.y), 3, 6};
            SDL_SetRenderDrawColor(sdl_renderer, 255, 215, 0, 255); // Gold
            SDL_RenderFillRect(sdl_renderer, &handle);
            break;
        }

        case TILE_WATER: // Water tiles (4)
        {
            SDL_Rect water_rect = {(int)(cmd.x - half_size), (int)(cmd.y - half_size), half_size * 2, half_size * 2};
            SDL_SetRenderDrawColor(sdl_renderer, 0, 100, 200, 255); // Blue
            SDL_RenderFillRect(sdl_renderer, &water_rect);

            SDL_SetRenderDrawColor(sdl_renderer, 0, 150, 255, 255); // Light blue
            SDL_RenderDrawLine(sdl_renderer, (int)(cmd.x - half_size), (int)(cmd.y - half_size / 2), (int)(cmd.x + half_size), (int)(cmd.y - half_size / 2));
            SDL_RenderDrawLine(sdl_renderer, (int)(cmd.x - half_size), (int)(cmd.y + half_size / 2), (int)(cmd.x + half_size), (int)(cmd.y + half_size / 2));
            break;
        }

        default:
            SDL_Rect debug_rect = {(int)(cmd.x - half_size), (int)(cmd.y - half_size), half_size * 2, half_size * 2};
            SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 255, 255); // Magenta
            SDL_RenderFillRect(sdl_renderer, &debug_rect);
            break;
        }
    }
}

void engine_end_frame()
{
    SDL_RenderPresent(sdl_renderer);
}

void engine_shutdown()
{
    // Let game/main.c handle SDL cleanup
    sdl_renderer = NULL;
}


void camera_init(Camera* cam, int screen_width, int screen_height) {
    cam->x = 0.0f;
    cam->y = 0.0f;
    cam->target_x = 0.0f;
    cam->target_y = 0.0f;
    cam->smoothing = 0.1f;  // Adjust for smoother/snappier following
    cam->screen_width = screen_width;
    cam->screen_height = screen_height;
}

void camera_follow(Camera* cam, float target_x, float target_y) {
    cam->target_x = target_x;
    cam->target_y = target_y;
}

void camera_update(Camera* cam, float dt) {
    // Smooth camera following using lerp
    float lerp_factor = 1.0f - powf(cam->smoothing, dt * 60.0f); // 60fps normalized
    
    cam->x += (cam->target_x - cam->x) * lerp_factor;
    cam->y += (cam->target_y - cam->y) * lerp_factor;
}

void camera_world_to_screen(Camera* cam, float world_x, float world_y, float* screen_x, float* screen_y) {
    *screen_x = world_x - cam->x + (cam->screen_width / 2.0f);
    *screen_y = world_y - cam->y + (cam->screen_height / 2.0f);
}