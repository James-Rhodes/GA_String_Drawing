#pragma once

#include "PopulationMember.hpp"
#include <iostream>
#include <array>
#include <chrono>
#include "raylib.h"
#include "rlgl.h"
#include "Dithering.h"
#include "Instrumentor.h"
#define CIRCLE_RESOLUTION 256
#define CIRCLE_RADIUS 200
#define NUM_LINES 100
#define BACKGROUND_COLOR BLACK
#define LINE_WIDTH 2
#define IMAGE_PATH "assets/stephanie-leblanc-JLMEZxBcXCU-unsplash.png"
#define POPULATION_SIZE 2000
#define MUTATION_RATE 0.1f
#define NUM_ELITE 200
#define ITERATIONS_UNTIL_SCREENSHOT 100

//#define PROFILING 1

#if PROFILING
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
#define PROFILE_FUNC() PROFILE_SCOPE(__FUNCTION__)
#else
#define PROFILE_SCOPE(name) 
#define PROFILE_FUNC()
#endif

namespace LineDraw {

    extern RenderTexture2D intermediateRender;
    extern RenderTexture2D currentRender;
    extern Texture2D textureToApproximate;



    struct LineIndices {
        int ptAIndex = -1;
        int ptBIndex = -1;
    };

    struct FitnessDetails {
        unsigned int distances[POPULATION_SIZE];
    };

    class LineDrawer : public GA_Cpp::PopulationMember<LineDrawer> {
    public:
        LineDrawer() = default;

        void Init() override;

        void CrossOver(const LineDrawer& parentA, const LineDrawer& parentB) override;

        void Mutate(float mutationRate) override;

        double CalculateFitness() override;

        void LogParameters() const override;

        void Draw() const;

        void SetComputeShaderCurrentIndexLoc(int loc);

        void UpdateAllFitness(const FitnessDetails& fitnessDetails);

        void SetUpStaticVariables();

        void SetUpComputeShader();

        static void SetPopulationPointer(std::vector<LineDrawer>* pointer);


    private:
        static std::array<Vector2, CIRCLE_RESOLUTION> s_lookupTable; // Look up for all LineDrawers for positions of lines
        static std::vector<Color> s_colorLookupTable; // Look up for all LineDrawers for colors of lines
        static int s_currFitnessIndex; // For finding out how many fitness's were calculated in the SSBO
        static int s_numFitnessCalculatedOn; // The number of members of the population that the fitness was calculated for (elite members don't have their fitness recalculated)
        static unsigned int s_computeShaderProgram; // Compute Shader
        static unsigned int s_ssboFitnessDetails; // The buffer id that will contain the fitness details. 
        static int s_computeShaderCurrentIndexLoc; // The loc of the current index that the fitness is being calculated for
        static std::vector<LineDrawer>* s_populationPointer; // A pointer to the array containing all of the members of the current ga population


        std::array<int,NUM_LINES> m_colorIndices;
        std::array<LineIndices,NUM_LINES> m_lineIndices;
    };



    void InitialiseTextures(const char* imagePath, const char* expectedOutputPath = "results/Expected_Output.png", const char* reducedColorPalettePath = "results/Reduced_Palette_Image.png");
    Texture2D GenerateTextureToApproximate(const char* imagePath);
}