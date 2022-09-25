#pragma once

#include "PopulationMember.hpp"
#include <iostream>
#include <array>
#include "raylib.h"
#define CIRCLE_RESOLUTION 256
#define CIRCLE_RADIUS 200
#define NUM_LINES 10
#define BACKGROUND_COLOR BLACK
#define LINE_WIDTH 2

namespace LineDraw {

    extern Color* pixelsToApproximate;

    extern RenderTexture2D intermediateRender;
    extern RenderTexture2D currentRender;


    struct LineIndices {
        int ptAIndex = -1;
        int ptBIndex = -1;
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