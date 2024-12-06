#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PADDLE_WIDTH 15
#define PADDLE_HEIGHT 60
#define BALL_SIZE 10

// Declare the scores
int player1_score = 0;
int player2_score = 0;

// Declare the paddle and ball positions
SDL_Rect paddle1, paddle2, ball;
int ball_vel_x = 5, ball_vel_y = 5;
int paddle1_vel = 0, paddle2_vel = 0;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// Initialize SDL and create a window and renderer
int init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    window = SDL_CreateWindow("Pong Game",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

// Reset the ball to the center
void reset_ball() {
    ball.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    ball_vel_x = (ball_vel_x > 0) ? 5 : -5;
    ball_vel_y = (ball_vel_y > 0) ? 5 : -5;
}

// Handle input and paddle movement
void handle_input() {
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    if (keystate[SDL_SCANCODE_W] && paddle1.y > 0)
        paddle1_vel = -10;
    else if (keystate[SDL_SCANCODE_S] && paddle1.y < SCREEN_HEIGHT - PADDLE_HEIGHT)
        paddle1_vel = 10;
    else
        paddle1_vel = 0;

    if (keystate[SDL_SCANCODE_UP] && paddle2.y > 0)
        paddle2_vel = -10;
    else if (keystate[SDL_SCANCODE_DOWN] && paddle2.y < SCREEN_HEIGHT - PADDLE_HEIGHT)
        paddle2_vel = 10;
    else
        paddle2_vel = 0;
}

// Update the game state
void update() {
    paddle1.y += paddle1_vel;
    paddle2.y += paddle2_vel;

    // Update the ball's position
    ball.x += ball_vel_x;
    ball.y += ball_vel_y;

    // Ball collision with top and bottom
    if (ball.y <= 0 || ball.y >= SCREEN_HEIGHT - BALL_SIZE) {
        ball_vel_y = -ball_vel_y;
    }

    // Ball collision with paddles
    if (SDL_HasIntersection(&ball, &paddle1) || SDL_HasIntersection(&ball, &paddle2)) {
        ball_vel_x = -ball_vel_x;
    }

    // Ball out of bounds (left or right)
    if (ball.x <= 0) {
        player2_score++;
        reset_ball();
    } else if (ball.x >= SCREEN_WIDTH - BALL_SIZE) {
        player1_score++;
        reset_ball();
    }

    // Check if either player wins
    if (player1_score >= 10) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Player 1 Wins!", "Player 1 is the first to 10!", window);
        player1_score = 0;
        player2_score = 0;
    } else if (player2_score >= 10) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Player 2 Wins!", "Player 2 is the first to 10!", window);
        player1_score = 0;
        player2_score = 0;
    }
}

// Render the game state to the screen
void render() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Set the color for paddles and ball
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Draw paddles and ball
    SDL_RenderFillRect(renderer, &paddle1);
    SDL_RenderFillRect(renderer, &paddle2);
    SDL_RenderFillRect(renderer, &ball);

    // Draw the scores
    char score_text[16];
    sprintf(score_text, "Player 1: %d", player1_score);
    // To display the score on the screen, you can use SDL_ttf or render text yourself
    // (left as a placeholder here for simplicity)
    SDL_RenderPresent(renderer);
}

// Clean up SDL resources
void close_game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    if (!init()) {
        printf("Failed to initialize!\n");
        return 1;
    }

    // Initialize paddles and ball
    paddle1 = (SDL_Rect){20, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
    paddle2 = (SDL_Rect){SCREEN_WIDTH - 20 - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
    ball = (SDL_Rect){SCREEN_WIDTH / 2 - BALL_SIZE / 2, SCREEN_HEIGHT / 2 - BALL_SIZE / 2, BALL_SIZE, BALL_SIZE};

    SDL_Event e;
    int quit = 0;

    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        handle_input();
        update();
        render();
        SDL_Delay(16);  // Control the frame rate (roughly 60 FPS)
    }

    close_game();
    return 0;
}
// gcc -o pong.exe main.c -I src/include -L src/lib -lmingw32 -lSDL2main -lSDL2 -mconsole