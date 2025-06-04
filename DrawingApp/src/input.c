#pragma once

#include "input.h"

void get_input(Input* input) {
    memcpy(input->wasKeyboard, input->isKeyboard, SDL_NUM_SCANCODES);
    memset(input->isMouseClick, false, sizeof(input->isMouseClick));
    input->isScroll = 0;

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_KEYDOWN:
            input->isKeyboard[ev.key.keysym.scancode] = true;
            break;
        case SDL_KEYUP:
            input->isKeyboard[ev.key.keysym.scancode] = false;
            break;
        case SDL_MOUSEWHEEL:
            if (ev.wheel.y > 0) {
                input->isScroll = 1;
            }
            if (ev.wheel.y < 0) {
                input->isScroll = -1;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            input->isMouse[ev.button.button] = true;
            input->isMouseClick[ev.button.button] = true;
            break;
        case SDL_MOUSEBUTTONUP:
            input->isMouse[ev.button.button] = false;
            input->isMouseClick[ev.button.button] = false;
            break;
        case SDL_MOUSEMOTION:
            input->mouseX = ev.motion.x;
            input->mouseY = ev.motion.y;
            break;
        case SDL_QUIT:
            input->isQuit = true;
            break;
        default:
            break;
        }
    }
}
