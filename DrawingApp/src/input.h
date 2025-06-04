#pragma once

#include <SDL.h>
#include <stdbool.h>

#define NUM_MOUSE_BTNS 8

typedef struct Input {
    bool isKeyboard[SDL_NUM_SCANCODES];
    bool wasKeyboard[SDL_NUM_SCANCODES];
    bool isMouse[NUM_MOUSE_BTNS];
    bool isMouseClick[NUM_MOUSE_BTNS];
    int mouseX;
    int mouseY;
    int isScroll;

    bool isQuit;
} Input;

void get_input(Input* input);