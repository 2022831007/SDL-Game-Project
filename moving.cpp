#include <SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int CIRCLE_RADIUS = 30;

// Circle structure
struct Circle {
    int x, y;
    int dx; // velocity in x-direction
};

// Function to check collision between two circles
bool checkCollision(Circle& circle1, Circle& circle2) {
    int totalRadius = CIRCLE_RADIUS * 2;
    int dx = circle1.x - circle2.x;
    int dy = circle1.y - circle2.y;
    int distanceSquared = dx * dx + dy * dy;
    return distanceSquared <= (totalRadius * totalRadius);
}

int main(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Circle Collision", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize circle positions and velocities
    Circle circle1 = {CIRCLE_RADIUS, SCREEN_HEIGHT / 2, 5};
    Circle circle2 = {SCREEN_WIDTH / 2, CIRCLE_RADIUS, 0};

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // While application is running
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // Handle key inputs for circle2 movement
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        circle2.y -= 5;
                        break;
                    case SDLK_DOWN:
                        circle2.y += 5;
                        break;
                    case SDLK_LEFT:
                        circle2.x -= 5;
                        break;
                    case SDLK_RIGHT:
                        circle2.x += 5;
                        break;
                }
            }
        }

        // Move circle1 from left to right continuously
        circle1.x += circle1.dx;
        if (circle1.x >= SCREEN_WIDTH - CIRCLE_RADIUS || circle1.x <= CIRCLE_RADIUS) {
            circle1.dx = -circle1.dx;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw circle1
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawCircle(renderer, circle1.x, circle1.y, CIRCLE_RADIUS);

        // Draw circle2
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderDrawCircle(renderer, circle2.x, circle2.y, CIRCLE_RADIUS);

        // Check for collision
        if (checkCollision(circle1, circle2)) {
            // You can add visual effects here
            std::cout << "Collision detected!" << std::endl;
        }

        // Update screen
        SDL_RenderPresent(renderer);
    }

    // Destroy window
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}
