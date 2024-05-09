#include <SDL2/SDL.h>
#include <iostream>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Initial circle parameters
int circleX = WINDOW_WIDTH / 2;
int circleY = WINDOW_HEIGHT / 2;
int initialRadius = 50;
int radius = initialRadius;

// SDL variables
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool initSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    window = SDL_CreateWindow("Resizable Circle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void closeSDL() {
    // Destroy renderer
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    // Destroy window
    SDL_DestroyWindow(window);
    window = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}

void drawCircle() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw circle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int w = -radius; w <= radius; ++w) {
        for (int h = -radius; h <= radius; ++h) {
            if (w * w + h * h <= radius * radius) {
                SDL_RenderDrawPoint(renderer, circleX + w, circleY + h);
            }
        }
    }

    // Render
    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]) {
    // Initialize SDL
    if (!initSDL()) {
        std::cerr << "Failed to initialize SDL!" << std::endl;
        return 1;
    }

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Increase radius over time
        radius += 1;

        // Check for collision with window borders
        if (circleX - radius < 0 || circleX + radius >= WINDOW_WIDTH ||
            circleY - radius < 0 || circleY + radius >= WINDOW_HEIGHT) {
            // Reset radius
            radius = initialRadius;
        }

        // Draw circle
        drawCircle();

        // Delay to control frame rate
        SDL_Delay(5);
    }

    // Close SDL
    closeSDL();

    return 0;
}
