#ifndef LIFE_H
#define LIFE_H

typedef struct world {
        int total_living;
        int living_delta;

        char** display;
        char** buffer;

        int rows;
        int cols;

        void (*life)(struct world* world);
} world;

void world_init(world* w, int rows, int cols, void (*life)(world* w));
void world_term(world* w);

void world_swap_buffers(world* w);
void world_update(world* w);

#endif

