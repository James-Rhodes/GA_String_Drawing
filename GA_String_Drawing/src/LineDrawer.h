#pragma once

#include "PopulationMember.hpp"
#include <iostream>
#include <array>
#include <chrono>
#include "raylib.h"
#include "rlgl.h"
#define CIRCLE_RESOLUTION 256
#define CIRCLE_RADIUS 200
#define NUM_LINES 500
#define BACKGROUND_COLOR BLACK
#define LINE_WIDTH 2

#define PROFILING_FITNESS_FUNC 

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
        std::array<Color,NUM_LINES> m_colors;
        std::array<LineIndices,NUM_LINES> m_lineIndices;
    };
}