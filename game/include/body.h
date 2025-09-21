#ifndef BODY_H
#define BODY_H

typedef struct {
    float x, y;
    float rotation; // in degrees  
    float scale;
    float speed;
    float vx, vy;
    int sprite_id;
} Body;

#endif