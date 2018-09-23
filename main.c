/**
 * @file main.c
 * @brief Entry point and rendering for Conway's Game of Life
 * @author Garrett Bates <garrett.bates@student.nmt.edu>
 */
#define _POSIX_C_SOURCE 199309L
#define NCURSES_WIDECHAR 1

#include <math.h>
#include <time.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <signal.h>
#include <ncursesw/ncurses.h>

#include "life.h"

#define WIDTH 80
#define HEIGHT 25
#define X 40
#define Y 10

#define BACKGROUND (L'\u2592')
#define CELL (L'\u2588')

/**
 * @brief Render the game to the console using a frame buffer of cchar_t
 * @param w pointer to the life's world structure containing game state
 * @param framebuf frame buffer
 */
void render(world* w, cchar_t** framebuf)
{
        int i;
        int j;
        for(i = 0; i < w->rows; i++) {
                for(j = 0; j < w->cols; j++) {
                        framebuf[i][j].attr = (w->display[i][j] == 1) ?
                               COLOR_PAIR(1) : COLOR_PAIR(2);
                        framebuf[i][j].chars[0] = (w->display[i][j] == 1) ?
                                CELL : BACKGROUND;
                }
                mvadd_wchnstr(i, 0, framebuf[i], w->cols);
        }
}

/**
 * @brief Performs the seeds rule (B2/S)
 * @param world pointer to the life's world structure containing game state
 */
void seeds(world* world)
{
        int i;
        int j;
        int rows;
        int cols;
        char** grid;
        char** buf;

        rows = world->rows;
        cols = world->cols;
        grid = world->display;
        buf = world->buffer;

        for(i = 0; i < rows; i++) {
                for(j = 0; j < cols; j++) {
                        int count;

                        count = 0;
                        buf[i][j] = 0;

                        if(i == 0 && j == 0) {
                                count += grid[i][j + 1];
                                count += grid[i + 1][j];
                                count += grid[i + 1][j + 1];

                                count += grid[rows - 1][j];
                        }
                        else if(i == rows - 1 && j == 0) {
                                count += grid[i][j + 1];
                                count += grid[i - 1][j];
                                count += grid[i - 1][j + 1];

                                count += grid[0][j];
                        }
                        else if(i == 0 && j == cols - 1) {
                                count += grid[i][j - 1];
                                count += grid[i + 1][j];
                                count += grid[i + 1][j - 1];

                                count += grid[i][0];
                        }
                        else if(i == rows - 1 && j == cols - 1) {
                                count += grid[i][j - 1];
                                count += grid[i - 1][j];
                                count += grid[i - 1][j - 1];

                                count += grid[i][cols - 1];
                        }
                        else if(i == 0 && j > 0 && j < cols) {
                                count += grid[i][j - 1];
                                count += grid[i + 1][j - 1];
                                count += grid[i + 1][j];
                                count += grid[i + 1][j + 1];
                                count += grid[i][j + 1];

                                count += grid[rows - 1][j];
                                count += grid[rows - 1][j - 1];
                                count += grid[rows - 1][j + 1];
                        }
                        else if(i == rows - 1 && j > 0 && j < cols) {
                                count += grid[i][j - 1];
                                count += grid[i - 1][j - 1];
                                count += grid[i - 1][j];
                                count += grid[i - 1][j + 1];
                                count += grid[i][j + 1];

                                count += grid[0][j];
                                count += grid[0][j - 1];
                                count += grid[0][j + 1];
                        }
                        else if(i > 0 && i < rows && j == 0) {
                                count += grid[i - 1][j];
                                count += grid[i - 1][j + 1];
                                count += grid[i][j + 1];
                                count += grid[i + 1][j + 1];
                                count += grid[i + 1][j];

                                count += grid[i][cols - 1];
                                count += grid[i - 1][cols - 1];
                                count += grid[i + 1][cols - 1];
                        }
                        else if(i > 0 && i < rows && j == cols - 1) {
                                count += grid[i - 1][j];
                                count += grid[i - 1][j - 1];
                                count += grid[i][j - 1];
                                count += grid[i + 1][j - 1];
                                count += grid[i + 1][j];

                                count += grid[i][0];
                                count += grid[i - 1][0];
                                count += grid[i + 1][0];
                        }
                        else {
                                count += grid[i][j + 1];
                                count += grid[i + 1][j + 1];
                                count += grid[i + 1][j];
                                count += grid[i + 1][j - 1];
                                count += grid[i][j - 1];
                                count += grid[i - 1][j - 1];
                                count += grid[i - 1][j];
                                count += grid[i - 1][j + 1];
                        }

                        if((count == 2 || count == 3) && grid[i][j] == 1) {
                                buf[i][j] = 1;
                        }
                        else if(count == 2 && grid[i][j] == 0) {
                                buf[i][j] = 1;
                        }

                        if(rand() % 2382 == 0) {
                                buf[i][j] = 1;
                        }

                }
        }
}

/**
 * @brief Perform an iteration of life!
 * @param world the front buffer being displayed to the user
 */
void standard(world* world)
{
        int i;
        int j;
        int rows;
        int cols;
        char** grid;
        char** buf;

        rows = world->rows;
        cols = world->cols;
        grid = world->display;
        buf = world->buffer;

        for(i = 0; i < rows; i++) {
                for(j = 0; j < cols; j++) {
                        int count;

                        count = 0;
                        buf[i][j] = 0;

                        if(i == 0 && j == 0) {
                                count += grid[i][j + 1];
                                count += grid[i + 1][j];
                                count += grid[i + 1][j + 1];

                                count += grid[rows - 1][j];
                        }
                        else if(i == rows - 1 && j == 0) {
                                count += grid[i][j + 1];
                                count += grid[i - 1][j];
                                count += grid[i - 1][j + 1];

                                count += grid[0][j];
                        }
                        else if(i == 0 && j == cols - 1) {
                                count += grid[i][j - 1];
                                count += grid[i + 1][j];
                                count += grid[i + 1][j - 1];

                                count += grid[i][0];
                        }
                        else if(i == rows - 1 && j == cols - 1) {
                                count += grid[i][j - 1];
                                count += grid[i - 1][j];
                                count += grid[i - 1][j - 1];

                                count += grid[i][cols - 1];
                        }
                        else if(i == 0 && j > 0 && j < cols) {
                                count += grid[i][j - 1];
                                count += grid[i + 1][j - 1];
                                count += grid[i + 1][j];
                                count += grid[i + 1][j + 1];
                                count += grid[i][j + 1];

                                count += grid[rows - 1][j];
                                count += grid[rows - 1][j - 1];
                                count += grid[rows - 1][j + 1];
                        }
                        else if(i == rows - 1 && j > 0 && j < cols) {
                                count += grid[i][j - 1];
                                count += grid[i - 1][j - 1];
                                count += grid[i - 1][j];
                                count += grid[i - 1][j + 1];
                                count += grid[i][j + 1];

                                count += grid[0][j];
                                count += grid[0][j - 1];
                                count += grid[0][j + 1];
                        }
                        else if(i > 0 && i < rows && j == 0) {
                                count += grid[i - 1][j];
                                count += grid[i - 1][j + 1];
                                count += grid[i][j + 1];
                                count += grid[i + 1][j + 1];
                                count += grid[i + 1][j];

                                count += grid[i][cols - 1];
                                count += grid[i - 1][cols - 1];
                                count += grid[i + 1][cols - 1];
                        }
                        else if(i > 0 && i < rows && j == cols - 1) {
                                count += grid[i - 1][j];
                                count += grid[i - 1][j - 1];
                                count += grid[i][j - 1];
                                count += grid[i + 1][j - 1];
                                count += grid[i + 1][j];

                                count += grid[i][0];
                                count += grid[i - 1][0];
                                count += grid[i + 1][0];
                        }
                        else {
                                count += grid[i][j + 1];
                                count += grid[i + 1][j + 1];
                                count += grid[i + 1][j];
                                count += grid[i + 1][j - 1];
                                count += grid[i][j - 1];
                                count += grid[i - 1][j - 1];
                                count += grid[i - 1][j];
                                count += grid[i - 1][j + 1];
                        }

                        if((count == 2 || count == 3) && grid[i][j] == 1) {
                                buf[i][j] = 1;
                        }
                        else if(count == 3 && grid[i][j] == 0) {
                                buf[i][j] = 1;
                        }

                        if(rand() % 2382 == 0) {
                                buf[i][j] = 1;
                        }
                }
        }
}

int main(int argc, char* argv[])
{
        int i;
        int fn;
        int step;
        int paused;
        cchar_t** framebuf;

        float dt;
        struct timespec pt;
        struct timespec ct;

        void (*rules[2])(world* w) = {
                standard,
                seeds
        };

        world w;

        srand(time(NULL));

        setlocale(LC_ALL, "");

        initscr();
        cbreak();
        timeout(0);
        noecho();
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_CYAN, COLOR_BLACK);

        framebuf = calloc(LINES, sizeof(cchar_t*));
        for(i = 0; i < LINES; i++) {
                framebuf[i] = calloc(COLS, sizeof(cchar_t));
        }

        signal(SIGINT, SIG_IGN);
        world_init(&w, LINES - 3, COLS, standard);

        w.display[1 + Y][1 + X] = 1;
        w.display[1 + Y][2 + X] = 1;
        w.display[1 + Y][3 + X] = 1;
        w.display[1 + Y][5 + X] = 1;
        w.display[2 + Y][1 + X] = 1;
        w.display[3 + Y][4 + X] = 1;
        w.display[3 + Y][5 + X] = 1;
        w.display[4 + Y][5 + X] = 1;
        w.display[4 + Y][3 + X] = 1;
        w.display[4 + Y][5 + X] = 1;
        w.display[5 + Y][1 + X] = 1;
        w.display[5 + Y][3 + X] = 1;
        w.display[5 + Y][5 + X] = 1;

        w.display[HEIGHT - (1 + Y)][WIDTH - (1 + X)] = 1;
        w.display[HEIGHT - (2 + Y)][WIDTH - (1 + X)] = 1;
        w.display[HEIGHT - (3 + Y)][WIDTH - (1 + X)] = 1;
        w.display[HEIGHT - (5 + Y)][WIDTH - (1 + X)] = 1;
        w.display[HEIGHT - (1 + Y)][WIDTH - (2 + X)] = 1;
        w.display[HEIGHT - (4 + Y)][WIDTH - (3 + X)] = 1;
        w.display[HEIGHT - (5 + Y)][WIDTH - (3 + X)] = 1;
        w.display[HEIGHT - (5 + Y)][WIDTH - (4 + X)] = 1;
        w.display[HEIGHT - (3 + Y)][WIDTH - (4 + X)] = 1;
        w.display[HEIGHT - (5 + Y)][WIDTH - (4 + X)] = 1;
        w.display[HEIGHT - (1 + Y)][WIDTH - (5 + X)] = 1;
        w.display[HEIGHT - (3 + Y)][WIDTH - (5 + X)] = 1;
        w.display[HEIGHT - (5 + Y)][WIDTH - (5 + X)] = 1;

        step = 0;
        paused = 1;
        mvprintw(LINES - 1, 0, "Press Q to Quit.");
        printw("   Press ] to Increase Speed.");
        mvprintw(LINES - 2, 0, "Press P to Pause.");
        printw("  Press [ to Decrease Speed.");
        mvprintw(LINES - 3, 0, "Press S to Step.");
        render(&w, framebuf);

        clock_gettime(CLOCK_MONOTONIC, &pt);
        clock_gettime(CLOCK_MONOTONIC, &ct);

        float accum = 0.0f;
        float tstep = 0.5f;

        fn = 0;

        while(1) {
                int i;
                i = getch();

                clock_gettime(CLOCK_MONOTONIC, &ct);
                dt = (float)(ct.tv_nsec - pt.tv_nsec) / 1000000000.0f;
                pt = ct;

                accum += fabs(dt * 7.0f);
                if(accum >= tstep) {

                        if(paused == 0 || step != 0) {
                                world_update(&w);
                                render(&w, framebuf);
                                refresh();
                                step = 0;
                        }
                        accum = 0.0f;
                }

                if(i == 'q') {
                        break;
                } else if(i == 'p') {
                        paused = !paused;
                } else if(i == 's') {
                        step = !step;
                } else if(i == '[') {
                        if(tstep < 10.0) {
                                tstep += 0.10f;
                        }
                } else if(i == ']') {
                        if(tstep > 0.0) {
                                tstep -= 0.10f;
                        }
                } else if(i == 'r') {
                        fn = (fn + 1) % (sizeof(rules) / sizeof(void*));
                        w.life = rules[fn];
                }
        }
        
        world_term(&w);
        for(i = 0; i < LINES; i++) {
                free(framebuf[i]);
        }
        free(framebuf);

        endwin();
}

