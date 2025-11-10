/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute 'raylib_compile_execute' script
*   Note that compiled executable is placed in the same folder as .c file
*
*   To test the examples on Web, press F6 and execute 'raylib_compile_execute_web' script
*   Web version of the program is generated in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include "raylib.h"
#include <stdlib.h> 
#include <stdio.h> 

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define TILE_SIZE 32
#define MAX_BODY 50
#define MAX_PLATFORMS 300
#define MAX_FOOD 10
#define GRID_WIDTH 25
#define GRID_HEIGHT 14

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

typedef struct Segment {
    Vector2 position; 
    Vector2 size;     
} Segment;

typedef struct Snakebird {
    Segment body[MAX_BODY];
    int length;
    Color color;
} Snakebird;

typedef struct Food {
    Vector2 position; // Position in TILE coordinates
    Vector2 size;     // Size in PIXELS
    bool active;
    Color color;
} Food;

typedef struct Platform {
    Rectangle rect; // Pixel coordinates of the platform
} Platform;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = GRID_WIDTH * TILE_SIZE;  
static const int screenHeight = GRID_HEIGHT * TILE_SIZE;

static Snakebird player = { 0 };
static Food food[MAX_FOOD] = { 0 };
static int foodCount = 0;
static Platform platforms[MAX_PLATFORMS] = { 0 };
static int platformCount = 0;
static Vector2 exitPosition = { 0, 0 }; 
static bool exitReached = false;       

static bool gameOver = false;
static bool pause = false;
static bool allowMove = false;  

const char* MAP[] = {
    "#########################",
    "#                       #",
    "#                       #",
    "#                       #",
    "#        FF             #",
    "#     #####     F       #", 
    "#     #   # F #####     #",
    "#           F # # #     #",
    "#       #       F       #",
    "#    #######    #       #",
    "#       #     #####     #",
    "#       #    E  #       #",
    "#    #              #   #", 
    "#########################"
};

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);
static void UpdateGame(void);
static void DrawGame(void);
static bool IsSolid(Vector2 tilePos);
static bool CheckCollisionWithBody(Vector2 tilePos);


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    InitWindow(screenWidth, screenHeight, "raylib snakebird");
    InitGame();
    SetTargetFPS(10); 

    while (!WindowShouldClose())
    {
        UpdateGame();
        DrawGame();
    }

    CloseWindow();
    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

void InitGame(void)
{
    gameOver = false;
    pause = false;
    exitReached = false; 
    allowMove = true;
    player.length = 1;
    player.color = DARKGREEN;

    int startX = 7;
    int startY = 3;
    player.body[0].position = (Vector2){(float)startX, (float)startY};
    player.body[0].size = (Vector2){ TILE_SIZE, TILE_SIZE };

    platformCount = 0;
    foodCount = 0;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (MAP[y][x] == '#') {
                platforms[platformCount].rect = (Rectangle){ x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                platformCount++;
            }
            else if (MAP[y][x] == 'F') {
                food[foodCount].position = (Vector2){(float)x, (float)y};
                food[foodCount].size = (Vector2){ TILE_SIZE, TILE_SIZE };
                food[foodCount].active = true; 
                foodCount++;
            }
            else if (MAP[y][x] == 'E') {
                exitPosition = (Vector2){(float)x, (float)y};
            }
        }
    }
}


bool IsSolid(Vector2 tilePos) {
    if (tilePos.x < 0 || tilePos.x >= GRID_WIDTH || 
        tilePos.y < 0 || tilePos.y >= GRID_HEIGHT || 
        MAP[(int)tilePos.y][(int)tilePos.x] == '#') {
        return true;
    }
    return false;
}

// Keep CheckCollisionWithFood separate for eating logic
bool CheckCollisionWithBody(Vector2 tilePos) {
    for (int i = 0; i < foodCount; i++) {
        if (food[i].active && food[i].position.x == tilePos.x && food[i].position.y == tilePos.y) {
            return true;
        }
    }
    return false;
}

void UpdateGame(void)
{
    //Win or Game Over
    if (gameOver && exitReached)
    {
         if (IsKeyPressed(KEY_ENTER)) InitGame();
    }
    else if (gameOver)
    {
        if (IsKeyPressed(KEY_ENTER)) InitGame();
    }
    else
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            //Gravity
            bool isFalling = false;
            Vector2 posBelow = {player.body[player.length-1].position.x, player.body[player.length-1].position.y + 1};
            if (!IsSolid(posBelow) && !CheckCollisionWithBody(posBelow)) isFalling = true;
            
            //Movement
            Vector2 desiredDirection = {0, 0};
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) desiredDirection = (Vector2){ 1, 0 };
            else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) desiredDirection = (Vector2){ -1, 0 };
            else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) desiredDirection = (Vector2){ 0, -1 };
            else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) desiredDirection = (Vector2){ 0, 1 };

            // --------------------------------------------------
            // Gravity vs Input
            // --------------------------------------------------

            Vector2 movementThisTurn = {0, 0};

            if (!isFalling)
            {
                movementThisTurn = desiredDirection;
            }
            else
            {
                movementThisTurn = (Vector2){0, 1};
            }

            if (movementThisTurn.x != 0 || movementThisTurn.y != 0)
            {
                Vector2 prevPos[MAX_BODY];
                for (int i = 0; i < player.length; i++) prevPos[i] = player.body[i].position;

                Vector2 nextHeadPos = {player.body[0].position.x + movementThisTurn.x, player.body[0].position.y + movementThisTurn.y};

                //Check collision before moving
                bool collisionWithBody = false;
                for(int i = 1; i < player.length; i++) {
                    if(player.body[i].position.x == nextHeadPos.x && player.body[i].position.y == nextHeadPos.y) {
                        collisionWithBody = true;
                        break;
                    }
                }
                

                if (IsSolid(nextHeadPos) || collisionWithBody) 
                {
                    //Do nothing, this cancels movement
                }
                else
                {
                    //Move all segments
                    for (int i = 1; i < player.length; i++) {
                        player.body[i].position = prevPos[i-1];
                    }
                    player.body[0].position = nextHeadPos;
                    
                    //Food
                    for (int i = 0; i < foodCount; i++) {
                        if (food[i].active && player.body[0].position.x == food[i].position.x &&
                            player.body[0].position.y == food[i].position.y)
                        {
                            if (player.length < MAX_BODY)
                            {
                                player.body[player.length].position = prevPos[player.length - 1]; 
                                player.body[player.length].size = (Vector2){TILE_SIZE, TILE_SIZE};
                                player.length += 1;
                            }
                            food[i].active = false; 
                            break; 
                        }
                    }

                    // Win
                    int remainingFood = 0;
                    for (int i = 0; i < foodCount; i++) {
                        if (food[i].active) {
                            remainingFood++;
                        }
                    }

                    if (remainingFood == 0 && player.body[0].position.x == exitPosition.x && 
                        player.body[0].position.y == exitPosition.y)
                    {
                       gameOver = true;
                       exitReached = true;
                    }
                }
            }
        }
    }
}

void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(SKYBLUE); 

    //Draw Platform
    for (int i = 0; i < platformCount; i++) {
        DrawRectangleRec(platforms[i].rect, BROWN);
    }

    //Draw Food
    for (int i = 0; i < foodCount; i++){
        if (food[i].active)
        {
             Vector2 drawPos = {
                food[i].position.x * TILE_SIZE,
                food[i].position.y * TILE_SIZE
            };
            DrawRectangleV(drawPos, food[i].size, YELLOW);
        }
    }

    //Draw exit (inactive, active pending)
    Vector2 exitDrawPos = {
        exitPosition.x * TILE_SIZE + TILE_SIZE/3, 
        exitPosition.y * TILE_SIZE + TILE_SIZE/3  
    };
    DrawCircleV(exitDrawPos, TILE_SIZE/2.0f, DARKGRAY);

    //Draw Snakebird
    for (int i = 0; i < player.length; i++)
    {
        Vector2 drawPos = {
            player.body[i].position.x * TILE_SIZE,
            player.body[i].position.y * TILE_SIZE
        };
        Color segmentColor = (i == 0) ? LIME : player.color;

        DrawRectangleV(drawPos, player.body[i].size, segmentColor);
        DrawRectangleLines(drawPos.x, drawPos.y, TILE_SIZE, TILE_SIZE, DARKGRAY);
    }
    

    if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);

    if (gameOver)
    {
        if (exitReached)
        {
            DrawText("LEVEL COMPLETE!", screenWidth/2 - MeasureText("LEVEL COMPLETE!", 40)/2, screenHeight/2 - 40, 40, GOLD);
        }
        else
        {
             DrawText("GAME OVER", screenWidth/2 - MeasureText("GAME OVER", 40)/2, screenHeight/2 - 40, 40, RED);
        }
        DrawText("PRESS [ENTER] TO PLAY AGAIN", screenWidth/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, screenHeight/2 + 10, 20, GRAY);
    }

    EndDrawing();
}
