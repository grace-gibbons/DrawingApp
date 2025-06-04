#pragma once

#include <SDL.h>
#include <stdbool.h>
#include "vec2.h"

// Size of the window created by SDL
#define WINDOW_WIDTH   1280
#define WINDOW_HEIGHT  720

// Pixel size of the window
#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  180

#define WINDOW_TITLE   "Drawing Program"

#define ZOOM_INC       0.25f

/*
* WindowManager
*
* topLeft The position of the top left corner of the canvas in the world
* dist The distance between the mouse and the pos; only when clicking to move canvas
*/
typedef struct WindowManager  {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t* pixels;
    int* winW;
    int* winH;

    float zoom;
    uint32_t drawColor;

    v2i topLeft;
    v2i dist;
} WinMan;

int init_win_man(WinMan* winMan);

void render_present(WinMan* winMan);

void free_win_man(WinMan* winMan);

///////////////////////////////////
// Util

// Convert a location on the screen (window) to an index into an array of pixels
int loc_to_index(v2i loc);

// Determine if the location is visible on the screen
bool loc_on_screen(v2i loc);