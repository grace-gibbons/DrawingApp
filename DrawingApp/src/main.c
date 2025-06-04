#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdbool.h> 
#include <string.h>
#include "window.h"
#include "input.h"
#include "vec2.h"

struct {
    WinMan winMan;
    Input input;
} state;

v2i mouse_to_screen() {
    v2i mouseLoc;

    int scaleX = *(state.winMan.winW) / (SCREEN_WIDTH / state.winMan.zoom);
    int scaleY = *(state.winMan.winH) / (SCREEN_HEIGHT / state.winMan.zoom);

    mouseLoc.i = (state.input.mouseX - state.winMan.topLeft.i) / scaleX;
    mouseLoc.j = (state.input.mouseY - state.winMan.topLeft.j) / scaleY;

    return mouseLoc;
}

void update() {
    v2i mouseLoc = mouse_to_screen();

    // Select color
    if (state.input.isKeyboard[SDL_SCANCODE_1]) {
        state.winMan.drawColor = 0xFF0000FF;
    } else if (state.input.isKeyboard[SDL_SCANCODE_2]) {
        state.winMan.drawColor = 0x00FF00FF;
    } else if (state.input.isKeyboard[SDL_SCANCODE_3]) {
        state.winMan.drawColor = 0x0000FFFF;
    }

    // Draw a pixel
    if (state.input.isMouse[SDL_BUTTON_LEFT]) {
        if (loc_on_screen(mouseLoc)) {
            state.winMan.pixels[loc_to_index(mouseLoc)] = state.winMan.drawColor;
        }
    }
    
    // Get some initial distance between the top left corner of the image and the mouse
    if (state.input.isMouseClick[SDL_BUTTON_MIDDLE]) {
        state.winMan.dist = (v2i){ state.input.mouseX - state.winMan.topLeft.i, state.input.mouseY - state.winMan.topLeft.j };
    }

    // Move the image, if mouse middle wheel
    if (state.input.isMouse[SDL_BUTTON_MIDDLE]) {
        state.winMan.topLeft = (v2i){ state.input.mouseX - state.winMan.dist.i, state.input.mouseY - state.winMan.dist.j };
    }

    // Handle scrolling
    if (state.input.isScroll > 0) {
        state.winMan.zoom += ZOOM_INC;
    }
    if (state.input.isScroll < 0) {
        if (state.winMan.zoom > ZOOM_INC) {
            state.winMan.zoom -= ZOOM_INC;
        }
    }
}


void render() {

}

void free_program() {
    free_win_man(&state.winMan);
}

int main(int argc, char** argv) {
    if (init_win_man(&state.winMan) != 0) {
        printf("Error initializing\n");
        return -1;
    }

    while (!state.input.isQuit) {
        get_input(&state.input);

        update(); 

        render();
        render_present(&state.winMan);
    }

    free_program();

	return 0;
}