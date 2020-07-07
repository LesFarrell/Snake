#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"

// Our sprites are 16x16 so declare as a constant.
#define SpriteSize 16

// Used by Raylib to store the sprite sheet and sounds.
Texture2D imgSprites;
Sound sndPickUp;
Sound sndCrash;

// Constants for the sprite locations within the sprite sheet.
Rectangle HEAD = {16, 0, SpriteSize, SpriteSize};
Rectangle BODY = {32, 0, SpriteSize, SpriteSize};
Rectangle FLOWER = {48, 0, SpriteSize, SpriteSize};
Rectangle DEAD = {64, 0, SpriteSize, SpriteSize};

// Variables to hold the Snake and Flower positions.
Vector2 SnakeSegments[1000];
Vector2 FlowerPos;
Vector2 pos = {208, 112};

// Some enums to make things easier to read.
enum BOOL
{
    False,
    True
};

enum Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Global variables and flags.
int LengthOfSnake = 0;
int DeadSnake = False;
int DeadSnakeSoundPlayed = False;
int Score = 0;
int HiScore = 150;
int SnakeDirection = LEFT;

// Function to generate random numbers within a passed range.
int RandomRange(int lower, int upper) { return (rand() % (upper - lower + 1)) + lower; }

// Function to draw the sprites, scores and game over message, etc.
void DrawScreen(int screenWidth, int screenHeight)
{
    char buffer[80] = {'\0'};

    // Draw the snake.
    for (int i = 1; i <= LengthOfSnake; i++)
    {
        if (DeadSnake == True)
            // If the snake is dead then draw the dead body.
            DrawTextureRec(imgSprites, DEAD, SnakeSegments[i], WHITE);
        else
            // Otherwise then draw the body.
            DrawTextureRec(imgSprites, BODY, SnakeSegments[i], WHITE);
    }
    DrawTextureRec(imgSprites, HEAD, SnakeSegments[0], WHITE);

    // Draw the Flower.
    DrawTextureRec(imgSprites, FLOWER, FlowerPos, WHITE);

    // Draw the scores.
    sprintf(buffer, "Current Score: %4d", Score);
    DrawText(buffer, 150, 2, 18, WHITE);
    sprintf(buffer, "High Score: %4d", HiScore);
    DrawText(buffer, 350, 2, 18, WHITE);

    // If the snake is dead show the restart message.
    if (DeadSnake == True)
    {
        sprintf(buffer, "Game Over - Press 'Space' to restart.", Score);
        DrawText(buffer, 85, screenHeight / 2, 24, BLUE);
    }
}

// Add the snakes new position to the front of the array of snake segments.
void MoveSnake(Vector2 NewPosition)
{
    // Move the segments up one position in the array.
    for (int i = LengthOfSnake + 1; i > 0; i--)
        SnakeSegments[i] = SnakeSegments[i - 1];

    // Add the new position to the front.
    SnakeSegments[0] = NewPosition;
}

// Set the games starting conditions.
int InitialiseGame()
{
    int count = 0;

    // Clear down the array
    for (int i = 0; i < 1000; i++)
    {
        SnakeSegments[i].x = -16;
        SnakeSegments[i].y = -16;
    }

    // Start length of the snake.
    LengthOfSnake = 4;

    // Set the postion defaults.
    int StartX = SpriteSize * 25;
    for (int i = StartX; i < (LengthOfSnake * SpriteSize) + StartX; i = i + SpriteSize)
    {
        SnakeSegments[count].x = i;
        SnakeSegments[0].y = 192;
        count++;
    }
    pos.x = SnakeSegments[0].x;
    pos.y = 192;

    FlowerPos.x = 64;
    FlowerPos.y = 192;

    // Reset the score and flags.
    Score = 0;
    DeadSnake = False;
    DeadSnakeSoundPlayed = False;
    SnakeDirection = LEFT;
}

// Main entry point.
int main(void)
{
    int x = 0;
    int y = 0;
    int FrameCounter = 0;
    int screenWidth = 40 * SpriteSize;
    int screenHeight = 25 * SpriteSize;

    // Initialise Raylib and create the game window.
    InitWindow(screenWidth, screenHeight, "Snake");

    InitAudioDevice();

    // Load the sprite sheet.
    imgSprites = LoadTexture("snake.png");

    // Load some sounds.
    sndPickUp = LoadSound("pickup.wav");
    sndCrash = LoadSound("crash.wav");

    // Tweak the volumes
    SetSoundVolume(sndPickUp, 0.15);
    SetSoundVolume(sndCrash, 0.15);

    // Target 60 FPS.
    SetTargetFPS(60);

    // Setup the game state.
    InitialiseGame();

    while (!WindowShouldClose())
    {
        // Tell Raylib to begin the drawing loop.
        BeginDrawing();

        // Increment the framecounter. (We use this for timings)
        FrameCounter++;

        // Clear the screen.
        ClearBackground(BLACK);

        // Did the snake hit the edge of the screen?
        if (SnakeSegments[0].x <= 0 || SnakeSegments[0].x >= screenWidth)
        {
            DeadSnake = True;
            if (DeadSnakeSoundPlayed == False)
                PlaySound(sndCrash);
            DeadSnakeSoundPlayed = True;
        }

        if (SnakeSegments[0].y <= 0 || SnakeSegments[0].y >= screenHeight)
        {
            DeadSnake = True;
            if (DeadSnakeSoundPlayed == False)
                PlaySound(sndCrash);
            DeadSnakeSoundPlayed = True;
        }

        // Did we hit the flower?
        if ((SnakeSegments[0].x == FlowerPos.x && SnakeSegments[0].y == FlowerPos.y))
        {
            PlaySound(sndPickUp);

            // Increase the length of the snake.
            LengthOfSnake++;

            // Increase the Score.
            Score++;

            // Is this a new high score?
            if (Score > HiScore)
                HiScore = Score;

            // Create a flower in a new position.
            x = RandomRange(SpriteSize, screenWidth - SpriteSize);
            y = RandomRange(SpriteSize, screenHeight - SpriteSize);
            FlowerPos.x = ((x + SpriteSize / 2) / SpriteSize) * SpriteSize;
            FlowerPos.y = ((y + SpriteSize / 2) / SpriteSize) * SpriteSize;
        }

        // Up date the snakes position once every six seconds.
        if (FrameCounter >= 60 / 6)
        {
            // Reset the framecounter.
            FrameCounter = 0;

            // Decide which direction to move the Snake.
            if (SnakeDirection == LEFT)
                pos.x = pos.x - SpriteSize;
            if (SnakeDirection == RIGHT)
                pos.x = pos.x + SpriteSize;
            if (SnakeDirection == UP)
                pos.y = pos.y - SpriteSize;
            if (SnakeDirection == DOWN)
                pos.y = pos.y + SpriteSize;

            // Has the snake tried to eat itself?
            for (int i = LengthOfSnake; i > 0; i--)
                if (SnakeSegments[i].x == SnakeSegments[0].x && SnakeSegments[i].y == SnakeSegments[0].y)
                {
                    DeadSnake = True;
                    if (DeadSnakeSoundPlayed == False)
                        PlaySound(sndCrash);
                    DeadSnakeSoundPlayed = True;
                }

            // Update the snakes position.
            if (DeadSnake == False)
                MoveSnake(pos);
        }

        // Process the keypresses.
        if (IsKeyPressed(KEY_LEFT))
            SnakeDirection = LEFT;
        if (IsKeyPressed(KEY_RIGHT))
            SnakeDirection = RIGHT;
        if (IsKeyPressed(KEY_UP))
            SnakeDirection = UP;
        if (IsKeyPressed(KEY_DOWN))
            SnakeDirection = DOWN;

        // If the snake is dead and Space was pressed then restart the game.
        if (IsKeyPressed(KEY_SPACE) && DeadSnake == True)
            InitialiseGame();

        // Redraw the screen.
        DrawScreen(screenWidth, screenHeight);

        // Tell Raylib we have finished drawing.
        EndDrawing();
    }

    CloseAudioDevice();

    // Close down Raylib.
    CloseWindow();

    // All done.
    return 0;
}
