#pragma once

#include "window.h"

int init_win_man(WinMan* winMan) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL failed to initialize: %s", SDL_GetError());
        return -1;
    }

    winMan->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!winMan->window) {
        printf("Failed to create SDL window: %s\n", SDL_GetError());
        return -1;
    }

    winMan->renderer = SDL_CreateRenderer(winMan->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!winMan->renderer) {
        printf("Failed to create SDL renderer: %s\n", SDL_GetError());
        return -1;
    }

    winMan->texture = SDL_CreateTexture(winMan->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!winMan->texture) {
        printf("Error creating window texture\n");
        return -1;
    }
    SDL_SetTextureBlendMode(winMan->texture, SDL_BLENDMODE_BLEND);

    winMan->pixels = (uint32_t*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    if (!winMan->pixels) {
        printf("Error allocating window pixels\n");
        return -1;
    }

    SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    if (!cursor) {
        printf("Error creating cursor\n");
        return -1; 
    }
    SDL_SetCursor(cursor);

    winMan->winW = malloc(sizeof(int));
    winMan->winH = malloc(sizeof(int));
    SDL_GetWindowSize(winMan->window, winMan->winW, winMan->winH);

    winMan->zoom = 1.0f;
    //winMan->topLeft = (v2i){ 0, 0 };
    winMan->drawColor = 0x000000FF;

    return 0;
}

void render_present(WinMan* winMan) {
    void* px;  // Pointer to locked pixels
    int pitch; // Length of onw row of bytes

    // Lock entire texture (write access only)
    SDL_LockTexture(winMan->texture, NULL, &px, &pitch);
    {
        // For each row of the screen
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            // Copy a state.pixels row to the px variable
            memcpy(&((uint8_t*)px)[y * pitch], &winMan->pixels[y * SCREEN_WIDTH], SCREEN_WIDTH * sizeof(float));
        }
    }
    // Unlock texture and apply changes
    SDL_UnlockTexture(winMan->texture);

    // Set render target to window
    SDL_SetRenderTarget(winMan->renderer, NULL);
    // No blend mode
    SDL_SetRenderDrawBlendMode(winMan->renderer, SDL_BLENDMODE_NONE);

    // Clear current target with render color
    SDL_RenderClear(winMan->renderer);
    // Copy the entire tetxure to the whole renderer 
    SDL_RenderCopy(winMan->renderer, winMan->texture, NULL, &(SDL_Rect){ winMan->topLeft.i, winMan->topLeft.j, WINDOW_WIDTH * winMan->zoom, WINDOW_HEIGHT * winMan->zoom });

    // Update screen with new rendering performed since last call
    SDL_RenderPresent(winMan->renderer);
}

void free_win_man(WinMan* winMan) {
    if (winMan->pixels) {
        free(winMan->pixels);
        winMan->pixels = NULL;
    }

    if (winMan->winW) {
        free(winMan->winW);
        winMan->winW = NULL;
    }

    if (winMan->winH) {
        free(winMan->winH);
        winMan->winH = NULL;
    }

    SDL_DestroyTexture(winMan->texture);
    SDL_DestroyRenderer(winMan->renderer);
    SDL_DestroyWindow(winMan->window);
}

int loc_to_index(v2i loc) {
    return loc.j * SCREEN_WIDTH + loc.i;
}

bool loc_on_screen(v2i loc) {
    if (loc.i >= 0 && loc.i < SCREEN_WIDTH) {
        if (loc.j >= 0 && loc.j < SCREEN_HEIGHT) {
            return true;
        }
    }
    return false;
}
