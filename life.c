/**
 * @file life.c
 * @brief manages the world structure and calls the life function
 * @author Garrett Bates <garrett.bates@student.nmt.edu>
 */
#include <stdlib.h>
#include <unistd.h>

#include "life.h"

/**
 * @brief allocates memory and sets life rules
 * @param world the front buffer being displayed to the user
 */
void world_init(world* world, int rows, int cols, void (*life)(struct world*))
{
        int i;
        int j;

        world->total_living = 0;
        world->living_delta = 0;
        world->display = malloc(rows * sizeof(char*));
        world->buffer = malloc(rows * sizeof(char*));

        for(i = 0; i < rows; i++) {
                world->display[i] = malloc(cols);
                world->buffer[i] = malloc(cols);
                for(j = 0; j < cols; j++) {
                        world->display[i][j] = 0;
                        world->buffer[i][j] = 0;
                }
        }
        world->rows = rows;
        world->cols = cols;
        world->life = life;
}

/**
 * @brief frees allocated memory
 * @param world the front buffer being displayed to the user
 */
void world_term(world* world)
{
        int i;
        for(i = 0; i < world->rows; i++) {
                free(world->display[i]);
                free(world->buffer[i]);
        }
        free(world->display);
        free(world->buffer);
}

/**
 * @brief updates the simulation and takes a tally of living cells
 * @param world the front buffer being displayed to the user
 */
void world_update(world* world)
{
        int i;
        int j;
        int total;
        int delta;
        char** tmp;

        total = 0;
        delta = 0;
        world->life(world);
        for(i = 0; i < world->rows; i++) {
                for(j = 0; j < world->cols; j++) {
                        total += world->buffer[i][j];
                }
        }

        delta = total - world->total_living;
        world->living_delta = delta;
        world->total_living = total;

        tmp = world->buffer;
        world->buffer = world->display;
        world->display = tmp;
}

/**
 * @brief Sets the simulation ruleset to a user provided callback
 * @param world the front buffer being displayed to the user
 */
void world_set_rule(world* w, void (*life)(world*))
{
        w->life = life;
}

/**
 * @brief swaps the back and front buffers for life simulation
 * @param world the front buffer being displayed to the user
 */
void world_swap_buffers(world* world)
{
        char** tmp;

        tmp = world->buffer;
        world->buffer = world->display;
        world->display = tmp;
}

