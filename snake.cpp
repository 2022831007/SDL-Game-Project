#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SNAKE_BLOCK_SIZE = 10;
const int FONT_SIZE = 28;
const SDL_Color WHITE = {255, 255, 255};
const SDL_Color RED = {255, 0, 0};
const SDL_Color SKY_BLUE = {135, 206, 235, 255};
const int SNAKE_INITIAL_LENGTH = 2;

// SDL Objects
SDL_Window*window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
SDL_Texture* backgroundTexture = NULL;

// Game States and Enums
enum GameState { MENU, PLAYING, GAME_OVER };
enum Direction { UP, DOWN, LEFT, RIGHT }; 

// Snake Structure
struct SnakeSegment {
    int x, y;
};

// // Global Variables
vector<SnakeSegment> snake;
 int foodX, foodY;
int score = 0;
 GameState gameState = MENU;
Direction snakeDirection = RIGHT; // Snake's initial direction
 bool isGameInitialized = false;   // Track if the game has been initialized

void renderText(const string& text, int x, int y, SDL_Color color) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

 void initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        exit(1);
    }

    if (TTF_Init() == -1) {
        cerr << "TTF could not initialize! TTF_Error: " << TTF_GetError() << endl;
        exit(1);
    }

    window = SDL_CreateWindow("Snake Game created by Fatema", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        exit(1);
    }

    font = TTF_OpenFont("arial/arial_bold.ttf", FONT_SIZE);
    if (font == NULL) {
        cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        exit(1);
    }
}

void loadBackgroundImage() {
    SDL_Surface* bgSurface = IMG_Load("snkea.png"); // Ensure you have an image file with this name.
    if (!bgSurface) {
        cerr << "Failed to load background image! IMG_Error: " << IMG_GetError() <<endl;
        exit(1);
    }

    backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
}



void handleButton(SDL_Event &e, bool &quit) {
    if (e.type == SDL_QUIT) {
        quit = true;
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if Start button is clicked
        if (x >= SCREEN_WIDTH / 2 - 80 && x <= SCREEN_WIDTH / 2 + 80 &&
            y >= SCREEN_HEIGHT / 2 - 70 && y <= SCREEN_HEIGHT / 2 - 20) {
            gameState = PLAYING; // Switch to the game
        }

        // Check if Quit button is clicked
        if (x >= SCREEN_WIDTH / 2 - 80 && x <= SCREEN_WIDTH / 2 + 80 &&
            y >= SCREEN_HEIGHT / 2 && y <= SCREEN_HEIGHT / 2 + 50) {
            quit = true; // Exit the game
        }
    }
}


void renderMenu() {
    // Render the background
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    // Define button rectangles
    SDL_Rect startButton = {SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 70, 160, 50}; // Start button
    SDL_Rect quitButton = {SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2, 160, 50};      // Quit button

    // Set button colors
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for the Start button
    SDL_RenderFillRect(renderer, &startButton);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for the Quit button
    SDL_RenderFillRect(renderer, &quitButton);

    // Render button text
    renderText("START", SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 55, WHITE); // Centered on Start button
    renderText("QUIT", SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 + 15, WHITE);  // Centered on Quit button
}




void renderGameOver() {
    // Render "Game Over" text
    renderText("GAME OVER!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 150, RED);

    // Render the final score
    renderText("FINAL SCORE: " + std::to_string(score), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, WHITE);

    // Define button areas with increased width for "Restart"
    SDL_Rect restartButton = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 50 }; // Increased width
    SDL_Rect quitButton = { SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 70, 160, 50 }; // Same size

    // Render "Restart" button
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
    SDL_RenderFillRect(renderer, &restartButton);
    renderText("RESTART", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 10, WHITE); // Centered text

    // Render "Quit" button
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
    SDL_RenderFillRect(renderer, &quitButton);
    renderText("QUIT", SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 + 80, WHITE); // Centered text
}


void spawnFood() {
    foodX = (rand() % (SCREEN_WIDTH / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE;
    foodY = (rand() % (SCREEN_HEIGHT / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE;
}

void resetGame() {
    snake.clear();
    for (int i = 0; i < SNAKE_INITIAL_LENGTH; ++i) {
        snake.push_back({SCREEN_WIDTH / 2 - i * SNAKE_BLOCK_SIZE, SCREEN_HEIGHT / 2});
    }
    snakeDirection = RIGHT;
    score = 0;
    isGameInitialized = true;
    spawnFood();
}

bool checkCollision() {
    SnakeSegment& head = snake[0];

    if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT) {
        return true;
    }

    for (size_t i = 1; i < snake.size(); ++i) {
        if (head.x == snake[i].x && head.y == snake[i].y) {
            return true;
        }
    }

    return false;
}

void updateGame() {
    for (size_t i = snake.size() - 1; i > 0; --i) {
        snake[i] = snake[i - 1];
    }

    switch (snakeDirection) {
        case UP:    snake[0].y -= SNAKE_BLOCK_SIZE; break;
        case DOWN:  snake[0].y += SNAKE_BLOCK_SIZE; break;
        case LEFT:  snake[0].x -= SNAKE_BLOCK_SIZE; break;
        case RIGHT: snake[0].x += SNAKE_BLOCK_SIZE; break;
    }

    if (snake[0].x == foodX && snake[0].y == foodY) {
        score += 5;
        spawnFood();
        snake.push_back({snake.back().x, snake.back().y});
    }

    if (checkCollision()) {
        gameState = GAME_OVER;
    }
}


void handleInput(SDL_Event& e, bool& quit) {
    if (e.type == SDL_QUIT) {
        quit = true;
        return;
    }

    int mouseX, mouseY;
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        SDL_GetMouseState(&mouseX, &mouseY);

        // Handle input based on game state
        if (gameState == MENU) {
            // Menu buttons
            SDL_Rect startButton = { SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 70, 160, 50 };
            SDL_Rect quitButton = { SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2, 160, 50 };

            if (mouseX >= startButton.x && mouseX <= startButton.x + startButton.w &&
                mouseY >= startButton.y && mouseY <= startButton.y + startButton.h) {
                resetGame();          // Initialize game state
                gameState = PLAYING;  // Start the game
            }

            if (mouseX >= quitButton.x && mouseX <= quitButton.x + quitButton.w &&
                mouseY >= quitButton.y && mouseY <= quitButton.y + quitButton.h) {
                quit = true; // Quit the game
            }
        } else if (gameState == GAME_OVER) {
            // Game Over buttons
            SDL_Rect restartButton = { SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2, 150, 80 };
            SDL_Rect quitButton = { SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 70, 160, 50 };

            if (mouseX >= restartButton.x && mouseX <= restartButton.x + restartButton.w &&
                mouseY >= restartButton.y && mouseY <= restartButton.y + restartButton.h) {
                resetGame();          // Reset game state
                gameState = PLAYING;  // Restart the game
            }

            if (mouseX >= quitButton.x && mouseX <= quitButton.x + quitButton.w &&
                mouseY >= quitButton.y && mouseY <= quitButton.y + quitButton.h) {
                quit = true; // Quit the game
            }
        }
    }

    // Handle keyboard input in the PLAYING state
    if (e.type == SDL_KEYDOWN && gameState == PLAYING) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                if (snakeDirection != DOWN) snakeDirection = UP;
                break;
            case SDLK_DOWN:
                if (snakeDirection != UP) snakeDirection = DOWN;
                break;
            case SDLK_LEFT:
                if (snakeDirection != RIGHT) snakeDirection = LEFT;
                break;
            case SDLK_RIGHT:
                if (snakeDirection != LEFT) snakeDirection = RIGHT;
                break;
        }
    }
}


void renderGame() {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (const auto& segment : snake) {
        SDL_Rect snakeRect = { segment.x, segment.y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE };
        SDL_RenderFillRect(renderer, &snakeRect);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect foodRect = { foodX, foodY, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE };
    SDL_RenderFillRect(renderer, &foodRect);

    renderText("Score: " + std::to_string(score), 10, 10, WHITE);
}

void cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(backgroundTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initSDL();
    loadBackgroundImage();

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            handleInput(e, quit);
        }

        switch (gameState) {
            case MENU:
                renderMenu();
                break;
            case PLAYING:
                updateGame();
                renderGame();
                break;
            case GAME_OVER:
                renderGameOver();
                break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    cleanup();
    return 0;
}
