#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create a window and renderer
    SDL_Window* window = SDL_CreateWindow("SDL Shapes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Clear the screen with a white color
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Set drawing color to red
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    // Draw a blue point
    SDL_RenderDrawPoint(renderer, 100, 100);

    // Draw a green line
    SDL_RenderDrawLine(renderer, 200, 200, 300, 300);

    // Draw a yellow rectangle
    SDL_Rect rect = {400, 400, 100, 100};
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);

    // Draw a cyan filled rectangle
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Present the renderer
    SDL_RenderPresent(renderer);

    // Wait for a key press and clean up
    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
                quit = true;
        }
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
