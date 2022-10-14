#include "raylib.h"
#include "LineDrawer.h"
#include "GeneticAlgorithm.hpp"
#include "Dithering.h"
#include <iostream>
#include "rlgl.h"
#include "Instrumentor.h"
// Below ensures that the extra radeon GPU on my laptop gets used instead of the built in.
// ----------------------------------------------------------------------
#ifdef _WIN32
#ifdef __cplusplus
extern "C"
{
#endif

    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif
#endif
// ---------------------------------------------------------------------




int main()
{
    //// Initialization
    ////--------------------------------------------------------------------------------------
#ifdef PROFILING
    Instrumentor::Get().BeginSession("GA_String_Drawing", "./results/results.json");
#endif
    const int screenWidth = 800;
    const int screenHeight = 480;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "GA String Drawing");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    SetTraceLogLevel(LOG_NONE);

    LineDraw::InitialiseTextures(IMAGE_PATH);

    int numIterations = 0;
    int numLinesDrawn = NUM_LINES;
    RenderTexture2D screenTexture = LoadRenderTexture(screenWidth, screenHeight); // Texture for screenshot
    ////--------------------------------------------------------------------------------------

    GA_Cpp::GeneticAlgorithm<LineDraw::LineDrawer> ga(POPULATION_SIZE, MUTATION_RATE,NUM_ELITE,false); // For Profiling
    ga.Init();
    LineDraw::LineDrawer::SetPopulationPointer(&ga.GetPopulationReference());
    ga.SetPruneFrequency(3,20);
    //// Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //    // Update
        //    //----------------------------------------------------------------------------------

        PROFILE_SCOPE("Main_Loop");

        ga.Optimise();

        auto best = ga.GetBestResult();
        best.LogParameters();

        if (numIterations % ITERATIONS_UNTIL_SCREENSHOT == 0 && numIterations != 0) {
            BeginTextureMode(LineDraw::currentRender);
            best.Draw();
            EndTextureMode();
            //ga.InitAll(); // Realised that reinitialising every single member of the population resulted in much worse performance because at a certain point the screen is filled up enough
            numLinesDrawn += NUM_LINES;
        }

        BeginTextureMode(screenTexture);
        ClearBackground(BLACK);

        //DrawTextureRec(textureToApproximate, { 0,0,(float)textureToApproximate.width,-(float)textureToApproximate.height }, {0,0},WHITE);
        DrawTextureRec(LineDraw::currentRender.texture, { 0,0,(float)LineDraw::currentRender.texture.width,(float)LineDraw::currentRender.texture.height }, {0,0},WHITE);

        best.Draw();

        DrawText(TextFormat("Lines Drawn: %d",numLinesDrawn),10,20,16,WHITE);
        EndTextureMode();

        //    //----------------------------------------------------------------------------------
        //    // Draw
        //    //----------------------------------------------------------------------------------
        BeginDrawing();

        DrawTextureRec(screenTexture.texture, { 0,0,(float)screenTexture.texture.width,-(float)screenTexture.texture.height }, { 0,0 }, WHITE);

        EndDrawing();

        if (numIterations == 1 || numIterations % ITERATIONS_UNTIL_SCREENSHOT == 0 && numIterations != 0) {

            // Saves screenshot of current progress
            std::string fileTitle = "./results/Iteration_" + std::to_string(numIterations) + ".png";

            Image screenShot = LoadImageFromTexture(screenTexture.texture);
            ImageFlipVertical(&screenShot);

            ExportImage(screenShot, fileTitle.c_str());

            UnloadImage(screenShot);
        }

        numIterations++;
        //    //----------------------------------------------------------------------------------
    }
#ifdef PROFILING
    Instrumentor::Get().EndSession();
#endif
    //// De-Initialization
    ////--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    ////--------------------------------------------------------------------------------------

    return 0;
}

