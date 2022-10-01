#include "raylib.h"
#include "LineDrawer.h"
#include "GeneticAlgorithm.hpp"
#include <iostream>
#include "rlgl.h"

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

#define IMAGE_PATH "assets/stephanie-leblanc-JLMEZxBcXCU-unsplash.png"
#define POPULATION_SIZE 5
#define MUTATION_RATE 0.05f

Texture2D GenerateTextureToApproximate(const char* imagePath);

int main()
{
    //// Initialization
    ////--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "GA String Drawing");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    SetTraceLogLevel(LOG_NONE);

    Texture2D textureToApproximate = GenerateTextureToApproximate(IMAGE_PATH);
    Image imageToApproximate = LoadImageFromTexture(textureToApproximate);
    ImageFlipVertical(&imageToApproximate);
    ExportImage(imageToApproximate,"results/Expected_Output.png"); // Save the result we want the GA to Approximate
    LineDraw::pixelsToApproximate = LoadImageColors(imageToApproximate); // Global (sorry) variable for Line Drawers to try and approximate
    UnloadImage(imageToApproximate);


    LineDraw::intermediateRender = LoadRenderTexture(screenWidth,screenHeight);

    LineDraw::currentRender = LoadRenderTexture(screenWidth, screenHeight);
    BeginTextureMode(LineDraw::currentRender);
    ClearBackground(BACKGROUND_COLOR); // Initialise current render with black screen

    DrawCircle(screenWidth/2,screenHeight/2,20,RED);

    EndTextureMode();

    // Remove below when going back from playing with compute shaders
    BeginTextureMode(LineDraw::intermediateRender);
    ClearBackground(BACKGROUND_COLOR); // Initialise current render with black screen
    EndTextureMode();

    //For most basic testing of compute shader. Going to try and transfer data from currentRender (Texture) -> intermediateRender (Texture)

    //GA_Cpp::GeneticAlgorithm<LineDraw::LineDrawer> ga(POPULATION_SIZE,MUTATION_RATE,50);
    //GA_Cpp::GeneticAlgorithm<LineDraw::LineDrawer> ga(POPULATION_SIZE, MUTATION_RATE); // For Profiling

    //ga.SetPruneFrequency(5,10);

    //int numIterations = 0;
    //UnloadTexture(textureToApproximate);
    ////--------------------------------------------------------------------------------------


    //Compile Compute Shader
    char* computeShaderCode = LoadFileText("shaders/CalculateFitness_ComputeShader.glsl");
    unsigned int computeShader = rlCompileShader(computeShaderCode, RL_COMPUTE_SHADER);
    unsigned int computeShaderProgram = rlLoadComputeShaderProgram(computeShader);
    UnloadFileText(computeShaderCode);

    //Set Shader Uniforms (Textures)
    rlEnableShader(computeShaderProgram);

    rlBindImageTexture(LineDraw::currentRender.texture.id, 0, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, true);
    rlBindImageTexture(LineDraw::intermediateRender.texture.id, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,false);

    rlComputeShaderDispatch(screenWidth / 16, screenHeight / 16, 1);
    rlDisableShader();

    //// Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //    // Update
        //    //----------------------------------------------------------------------------------
  /*      ga.Optimise();

        auto best = ga.GetBestResult();
        best.LogParameters();

        if (numIterations % 100 == 0) {
            BeginTextureMode(LineDraw::currentRender);
            best.Draw();
            EndTextureMode();
            ga.InitAll();
        }*/
        //    //----------------------------------------------------------------------------------
        //    // Draw
        //    //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        //DrawTextureRec(textureToApproximate, { 0,0,(float)textureToApproximate.width,-(float)textureToApproximate.height }, {0,0},WHITE);
        //DrawTextureRec(LineDraw::currentRender.texture, { 0,0,(float)LineDraw::currentRender.texture.width,-(float)LineDraw::currentRender.texture.height }, {0,0},WHITE);
        DrawTextureRec(LineDraw::intermediateRender.texture, { 0,0,(float)LineDraw::intermediateRender.texture.width,-(float)LineDraw::intermediateRender.texture.height }, { 0,0 }, WHITE);

        //best.Draw();

        DrawFPS(10, 10);

        EndDrawing();
        //numIterations++;
        //    //----------------------------------------------------------------------------------
    }

    //// De-Initialization
    ////--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    ////--------------------------------------------------------------------------------------

    return 0;
}

Texture2D GenerateTextureToApproximate(const char* imagePath) {
    Image image = LoadImage(imagePath);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Texture2D texture = LoadTextureFromImage(image);

    Vector2 texCoords[CIRCLE_RESOLUTION + 1];
    Vector2 circlePoints[CIRCLE_RESOLUTION + 1];

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    RenderTexture2D renderTexture = LoadRenderTexture(screenWidth, screenHeight);


    float angleDelta = 2 * PI / CIRCLE_RESOLUTION;
    for (int i = 0; i < CIRCLE_RESOLUTION + 1; i++) {
        texCoords[i].x = 0.5 * cos(-angleDelta * i) + 0.5;
        texCoords[i].y = 0.5 * sin(-angleDelta * i) + 0.5;

        circlePoints[i].x = CIRCLE_RADIUS * cos(-angleDelta * i);
        circlePoints[i].y = CIRCLE_RADIUS * sin(-angleDelta * i);
    }
    BeginTextureMode(renderTexture);
    ClearBackground(BLACK);
    DrawTexturePoly(texture, { (float)screenWidth / 2,(float)screenHeight / 2 }, circlePoints, texCoords, CIRCLE_RESOLUTION + 1, WHITE);

    EndTextureMode();

    UnloadImage(image);
    UnloadTexture(texture);

    return renderTexture.texture;
}