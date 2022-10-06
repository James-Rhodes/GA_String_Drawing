#include "raylib.h"
#include "LineDrawer.h"
#include "GeneticAlgorithm.hpp"
#include "Dithering.h"
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
#define POPULATION_SIZE 2000
#define MUTATION_RATE 0.1f

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
    UnloadImage(imageToApproximate);

    Texture2D reducedPaletteTexture = CreateReducedColorPaletteTexture(textureToApproximate, 4);
    Image reducedPaletteImage = LoadImageFromTexture(reducedPaletteTexture);
    ExportImage(reducedPaletteImage, "results/Reduced_Palette_Image.png"); // Save the result we want the GA to Approximate
    UnloadImage(reducedPaletteImage);


    LineDraw::textureToApproximate = reducedPaletteTexture;
    LineDraw::intermediateRender = LoadRenderTexture(screenWidth,screenHeight);
    UnloadTexture(textureToApproximate);

    LineDraw::currentRender = LoadRenderTexture(screenWidth, screenHeight);
    BeginTextureMode(LineDraw::currentRender);
    ClearBackground(BLACK); // Initialise current render with black screen
    EndTextureMode();

    int numIterations = 1;
    int numLinesDrawn = NUM_LINES;
    ////--------------------------------------------------------------------------------------

    //Compile Compute Shader
    char* computeShaderCode = LoadFileText("shaders/CalculateFitness_ComputeShader.glsl");
    unsigned int computeShader = rlCompileShader(computeShaderCode, RL_COMPUTE_SHADER);
    LineDraw::computeShaderProgram = rlLoadComputeShaderProgram(computeShader);
    UnloadFileText(computeShaderCode);

    // Get Storage Buffer ID
    LineDraw::ssboFitnessDetails = rlLoadShaderBuffer(sizeof(LineDraw::FitnessDetails), NULL, RL_DYNAMIC_COPY);

    GA_Cpp::GeneticAlgorithm<LineDraw::LineDrawer> ga(POPULATION_SIZE, MUTATION_RATE,100); // For Profiling
    ga.SetPruneFrequency(3,20);
    //// Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //    // Update
        //    //----------------------------------------------------------------------------------
        ga.Optimise();

        auto best = ga.GetBestResult();
        best.LogParameters();

        if (numIterations % 25 == 0) {
            BeginTextureMode(LineDraw::currentRender);
            best.Draw();
            EndTextureMode();
            ga.InitAll();
            numLinesDrawn += NUM_LINES;
        }
        //    //----------------------------------------------------------------------------------
        //    // Draw
        //    //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        //DrawTextureRec(textureToApproximate, { 0,0,(float)textureToApproximate.width,-(float)textureToApproximate.height }, {0,0},WHITE);
        DrawTextureRec(LineDraw::currentRender.texture, { 0,0,(float)LineDraw::currentRender.texture.width,-(float)LineDraw::currentRender.texture.height }, {0,0},WHITE);

        best.Draw();

        DrawText(TextFormat("Lines Drawn: %d",numLinesDrawn),10,50,16,WHITE);
        DrawFPS(10, 10);

        EndDrawing();
        numIterations++;
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
        texCoords[i].x = 0.5f * cos(-angleDelta * i) + 0.5f;
        texCoords[i].y = 0.5f * sin(-angleDelta * i) + 0.5f;

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