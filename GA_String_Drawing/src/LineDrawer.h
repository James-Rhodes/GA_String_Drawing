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

    extern unsigned int computeShaderProgram; // Compute Shader
    extern unsigned int ssboFitnessDetails; // The buffer id that will contain the fitness details.


    struct LineIndices {
        int ptAIndex = -1;
        int ptBIndex = -1;
    };

    struct FitnessDetails {
        unsigned int distance = 0;
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


    private:
        static std::array<Vector2, CIRCLE_RESOLUTION> s_lookupTable; // Look up for all LineDrawers for positions of lines
        static std::vector<Color> s_colorLookupTable; // Look up for all LineDrawers for colors of lines

        std::array<int,NUM_LINES> m_colorIndices;
        std::array<LineIndices,NUM_LINES> m_lineIndices;
    };
}