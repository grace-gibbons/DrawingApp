#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdbool.h> 
#include <string.h>

// Size of the window created by SDL
#define WINDOW_WIDTH   1280
#define WINDOW_HEIGHT  720

// Pixel size of the window
#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  180

#define WINDOW_TITLE   "Drawing Program"

#define NUM_MOUSE_BTNS 8

#define ZOOM_INC       0.25f

typedef struct v2f { float i, j; } v2;
typedef struct v2i { int i, j; } v2i;

struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t* pixels;
    int* winW;
    int* winH;
    bool isFullscreen;
    bool quit;
    float zoom;
    v2i topLeft;

    v2i dist;
} state;

struct {
    bool isKeyboard[SDL_NUM_SCANCODES];
    bool wasKeyboard[SDL_NUM_SCANCODES];
    bool isMouse[NUM_MOUSE_BTNS];
    bool isMouseClick[NUM_MOUSE_BTNS];
    int mouseX;
    int mouseY;
    int isScroll;
} input;

int loc_to_index(v2i loc) {
    return loc.j * SCREEN_WIDTH + loc.i;
}

int init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL failed to initialize: %s", SDL_GetError());
        return -1;
    }

    state.window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!state.window) {
        printf("Failed to create SDL window: %s\n", SDL_GetError());
        return -1;
    }

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!state.renderer) {
        printf("Failed to create SDL renderer: %s\n", SDL_GetError());
        return -1;
    }

    state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!state.texture) {
        printf("Error creating window texture\n");
        return -1;
    }
    SDL_SetTextureBlendMode(state.texture, SDL_BLENDMODE_BLEND);

    state.pixels = (uint32_t*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    if (!state.pixels) {
        printf("Error allocating window pixels\n");
        return -1;
    }

    SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    SDL_SetCursor(cursor);

    state.winW = malloc(sizeof(int));
    state.winH = malloc(sizeof(int));
    SDL_GetWindowSize(state.window, state.winW, state.winH);
    state.isFullscreen = false;
    state.quit = false;
    state.zoom = 1.0f;
    state.topLeft = (v2i){ 0, 0 };

    return 0;
}

void get_input() {
    memcpy(input.wasKeyboard, input.isKeyboard, SDL_NUM_SCANCODES);
    memset(input.isMouseClick, false, sizeof(input.isMouseClick));
    input.isScroll = 0;

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_KEYDOWN:
            input.isKeyboard[ev.key.keysym.scancode] = true;
            break;
        case SDL_KEYUP:
            input.isKeyboard[ev.key.keysym.scancode] = false;
            break;
        case SDL_MOUSEWHEEL:
            if (ev.wheel.y > 0) {
                input.isScroll = 1;
            }
            if (ev.wheel.y < 0) {
                input.isScroll = -1;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            input.isMouse[ev.button.button] = true;
            input.isMouseClick[ev.button.button] = true;
            break;
        case SDL_MOUSEBUTTONUP:
            input.isMouse[ev.button.button] = false;
            input.isMouseClick[ev.button.button] = false;
            break;
        case SDL_MOUSEMOTION:
            input.mouseX = ev.motion.x;
            input.mouseY = ev.motion.y;
            break;
        case SDL_QUIT:
            state.quit = true;
            break;
        default:
            break;
        }
    }
}

v2i mouse_to_screen() {
    v2i mouseLoc;

    int scaleX = *state.winW / (SCREEN_WIDTH / state.zoom); // Needs fixing for error
    int scaleY = *state.winH / (SCREEN_HEIGHT / state.zoom);

    mouseLoc.i = (input.mouseX - state.topLeft.i - (input.mouseX % scaleX)) / scaleX;
    mouseLoc.j = (input.mouseY - state.topLeft.j - (input.mouseY % scaleY)) / scaleY;

    return mouseLoc;
}

bool loc_on_screen(v2i loc) {
    if (loc.i >= 0 && loc.i < SCREEN_WIDTH) {
        if (loc.j >= 0 && loc.j < SCREEN_HEIGHT) {
            return true;
        }
    }
    return false;
}

void update() {
    v2i mouseLoc = mouse_to_screen();

    // Draw a pixel
    if (input.isMouse[SDL_BUTTON_LEFT]) {
        if (loc_on_screen(mouseLoc)) {
            state.pixels[loc_to_index(mouseLoc)] = 0xFF0000FF;
        }
    }

    // Get some initial distance between the top left corner of the image and the mouse
    if (input.isMouseClick[SDL_BUTTON_MIDDLE]) {
        state.dist = (v2i){ input.mouseX - state.topLeft.i, input.mouseY - state.topLeft.j };
        printf("%d %d\n", state.dist.i, state.dist.j);
    }

    // Move the image, if mouse clicked
    if (input.isMouse[SDL_BUTTON_MIDDLE]) {
        state.topLeft = (v2i){ input.mouseX - state.dist.i, input.mouseY - state.dist.j };
    }

    // Handle scrolling
    if (input.isScroll > 0) {
        state.zoom += ZOOM_INC;
    }
    if (input.isScroll < 0) {
        if (state.zoom > ZOOM_INC) {
            state.zoom -= ZOOM_INC;
        }
    }
}

void render_present() {
    void* px;  // Pointer to locked pixels
    int pitch; // Length of onw row of bytes

    // Lock entire texture (write access only)
    SDL_LockTexture(state.texture, NULL, &px, &pitch);
    {
        // For each row of the screen
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            // Copy a state.pixels row to the px variable
            memcpy(&((uint8_t*)px)[y * pitch], &state.pixels[y * SCREEN_WIDTH], SCREEN_WIDTH * sizeof(float));
        }
    }
    // Unlock texture and apply changes
    SDL_UnlockTexture(state.texture);

    // Set render target to window
    SDL_SetRenderTarget(state.renderer, NULL);
    // No blend mode
    SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_NONE);

    // Clear current target with render color
    SDL_RenderClear(state.renderer);
    // Copy the entire tetxure to the whole renderer 
    SDL_RenderCopy(state.renderer, state.texture, NULL, &(SDL_Rect){ state.topLeft.i, state.topLeft.j, WINDOW_WIDTH * state.zoom, WINDOW_HEIGHT * state.zoom });

    // Update screen with new rendering performed since last call
    SDL_RenderPresent(state.renderer);
}

void render() {

}

void free_all() {
    if (state.pixels) {
        free(state.pixels);
        state.pixels = NULL;
    }

    if (state.winW) {
        free(state.winW);
        state.winW = NULL;
    }

    if (state.winH) {
        free(state.winH);
        state.winH = NULL;
    }

    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
}

int main(int argc, char** argv) {
    if (init() != 0) {
        printf("Error initializing\n");
        return -1;
    }

    while (!state.quit) {
        get_input();

        if (state.quit) {
            break;
        }

        update(); 

        render();
        render_present();
    }

    free_all();

	return 0;
}