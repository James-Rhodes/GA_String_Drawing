#include "LineDrawer.h"

std::array<Vector2, CIRCLE_RESOLUTION> LineDraw::LineDrawer::s_lookupTable;
Color* LineDraw::pixelsToApproximate = nullptr;

void LineDraw::LineDrawer::Init()
{
	if (s_lookupTable[0].x == 0 && s_lookupTable[0].y == 0) { 
		// If the look up table hasn't been initialised then initialise it
		std::cout << "Lookup Table Initialised" << std::endl;

		float angleDelta = 2 * PI / CIRCLE_RESOLUTION;

		for (size_t i = 0; i < s_lookupTable.size(); i++) {
			s_lookupTable[i].x = CIRCLE_RADIUS * cos(angleDelta * i) + 0.5 * GetScreenWidth();
			s_lookupTable[i].y = CIRCLE_RADIUS * sin(angleDelta * i) + 0.5 * GetScreenHeight();
		}
	}


	for (size_t i = 0; i < m_lineIndices.size(); i++) {
		// Initialising with random values
		m_lineIndices[i].ptAIndex = GA_Cpp::GetRandomInt(0, CIRCLE_RESOLUTION - 1);
		m_lineIndices[i].ptBIndex = GA_Cpp::GetRandomInt(0, CIRCLE_RESOLUTION - 1);

		m_colors[i].r = GA_Cpp::GetRandomInt(0, 255);
		m_colors[i].g = GA_Cpp::GetRandomInt(0, 255);
		m_colors[i].b = GA_Cpp::GetRandomInt(0, 255);
		m_colors[i].a = 255;

	}
}

void LineDraw::LineDrawer::CrossOver(const LineDrawer& parentA, const LineDrawer& parentB)
{
}

void LineDraw::LineDrawer::Mutate(float mutationRate)
{
}

double LineDraw::LineDrawer::CalculateFitness()
{
	return 0.0;
}

void LineDraw::LineDrawer::LogParameters() const
{
}

void LineDraw::LineDrawer::Draw() const
{
	for (Vector2 pt : s_lookupTable) {
		DrawCircleV(pt, 2, RED);
	}

	for (size_t i = 0; i < m_lineIndices.size(); i++) {
		DrawLineEx(s_lookupTable[m_lineIndices[i].ptAIndex], s_lookupTable[m_lineIndices[i].ptBIndex], LINE_WIDTH, m_colors[i]);
	}
}
