#include "raylib.h"
#include <iostream>

int main()
{
    //// Initialization
    ////--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Raylib Template");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    ////--------------------------------------------------------------------------------------

    //// Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //    // Update
        //    //----------------------------------------------------------------------------------

        //    //----------------------------------------------------------------------------------
        //    // Draw
        //    //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        const char *exampleText = "Hello World - Raylib LOLOLOLOL";
        int textWidth = MeasureText(exampleText, 20);
        DrawText(exampleText, (screenWidth / 2) - (textWidth / 2), screenHeight / 2 - 10, 20, BLACK);

        DrawFPS(10, 10);

        EndDrawing();
        //    //----------------------------------------------------------------------------------
    }

    //// De-Initialization
    ////--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    ////--------------------------------------------------------------------------------------

    std::cout << "Hello my dude" << std::endl;

    return 0;
}