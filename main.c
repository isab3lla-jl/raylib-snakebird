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
#define MAX_PLATFORMS 100
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
static Food food = { 0 };
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
    "#                       #",
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
//static void UpdateGame(void);
static void DrawGame(void);
//static bool CheckCollisionWithPlatforms(Vector2 tilePos);
//static bool CheckCollisionWithBody(Vector2 tilePos);


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    InitWindow(screenWidth, screenHeight, "raylib snakebird (Map Objects & Collisions)");
    InitGame();
    SetTargetFPS(10); 

    while (!WindowShouldClose())
    {
        //UpdateGame();
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
    
    player.body[0].position = (Vector2){ GRID_WIDTH/2, GRID_HEIGHT/2 };
    player.body[0].size = (Vector2){ TILE_SIZE, TILE_SIZE };

    platformCount = 0;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (MAP[y][x] == '#') {
                platforms[platformCount].rect = (Rectangle){ x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                platformCount++;
            }
            else if (MAP[y][x] == 'F') {
                food.position = (Vector2){(float)x, (float)y};
                food.size = (Vector2){ TILE_SIZE, TILE_SIZE };
                food.active = true; 
            }
            else if (MAP[y][x] == 'E') {
                exitPosition = (Vector2){(float)x, (float)y};
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
    if (food.active)
    {
         Vector2 drawPos = {
            food.position.x * TILE_SIZE,
            food.position.y * TILE_SIZE
        };
        DrawRectangleV(drawPos, food.size, YELLOW);
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
