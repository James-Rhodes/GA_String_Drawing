#pragma once

#include "PopulationMember.hpp"
#include <iostream>
#define CIRCLE_RESOLUTION 256
#define CIRCLE_RADIUS 200

#define LINE_WIDTH 2

class LineDrawer : public GA_Cpp::PopulationMember<LineDrawer> {
public:
	LineDrawer() = default;

    void Init() override;

    void CrossOver(const LineDrawer& parentA, const LineDrawer& parentB) override;

    void Mutate(float mutationRate) override;

    double CalculateFitness() override;

    void LogParameters() const override;
};