#include "LineDrawer.h"


std::array<Vector2, CIRCLE_RESOLUTION> LineDraw::LineDrawer::s_lookupTable;
Color* LineDraw::pixelsToApproximate = nullptr;
RenderTexture LineDraw::intermediateRender;
RenderTexture LineDraw::currentRender;


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
	size_t crossOverPoint = m_lineIndices.size() / 2; // Simple midpoint crossover
	for (size_t i = 0; i < m_lineIndices.size(); i++) {
		if (i < crossOverPoint) {
			m_lineIndices[i].ptAIndex = parentA.m_lineIndices[i].ptAIndex;
			m_lineIndices[i].ptBIndex = parentA.m_lineIndices[i].ptBIndex;

			m_colors[i] = parentA.m_colors[i];
		}
		else {
			m_lineIndices[i].ptAIndex = parentB.m_lineIndices[i].ptAIndex;
			m_lineIndices[i].ptBIndex = parentB.m_lineIndices[i].ptBIndex;

			m_colors[i] = parentB.m_colors[i];
		}
	}
}

void LineDraw::LineDrawer::Mutate(float mutationRate)
{

	for (size_t i = 0; i < m_lineIndices.size(); i++) {

		m_lineIndices[i].ptAIndex = (m_lineIndices[i].ptAIndex + (int)(CIRCLE_RESOLUTION * GA_Cpp::GetRandom01())) % CIRCLE_RESOLUTION;
		m_lineIndices[i].ptBIndex = (m_lineIndices[i].ptBIndex + (int)(CIRCLE_RESOLUTION * GA_Cpp::GetRandom01())) % CIRCLE_RESOLUTION;

		m_colors[i].r = (m_colors[i].r + (int)(255 * GA_Cpp::GetRandom01())) % 255;
		m_colors[i].g = (m_colors[i].g + (int)(255 * GA_Cpp::GetRandom01())) % 255;
		m_colors[i].b = (m_colors[i].b + (int)(255 * GA_Cpp::GetRandom01())) % 255;

	}
}

double LineDraw::LineDrawer::CalculateFitness()
{
#ifdef PROFILING_FITNESS_FUNC
	auto beginDraw = std::chrono::high_resolution_clock::now();
#endif
	BeginTextureMode(LineDraw::intermediateRender);
	ClearBackground(BACKGROUND_COLOR);

	DrawTextureRec(LineDraw::currentRender.texture, { 0,0,(float)LineDraw::currentRender.texture.width,-(float)LineDraw::currentRender.texture.height }, { 0,0 }, WHITE);

	Draw();

	EndTextureMode();
#ifdef PROFILING_FITNESS_FUNC
	auto endDraw = std::chrono::high_resolution_clock::now();
	auto beginLoadingData = std::chrono::high_resolution_clock::now();
#endif
	Image image = LoadImageFromTexture(LineDraw::intermediateRender.texture);
	Color* pixels = LoadImageColors(image);
#ifdef PROFILING_FITNESS_FUNC
	auto endLoadingData = std::chrono::high_resolution_clock::now();


	auto beginCalculation = std::chrono::high_resolution_clock::now();
#endif

	int hWidth = GetScreenWidth() * 0.5;
	int hHeight = GetScreenHeight() * 0.5;
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();

	double cumulativeDistance = 0;

	for (int y = 0; y < screenHeight; y++) {
		for (int x = 0; x < screenWidth; x++) {

			float distFromCenter = (x - hWidth) * (x - hWidth) + (y - hHeight) * (y - hHeight);
			if (distFromCenter < CIRCLE_RADIUS * CIRCLE_RADIUS) {
				Color& fromImage = LineDraw::pixelsToApproximate[x + y * screenWidth];
				Color& fromDrawer = pixels[x + y * screenWidth];

				float redDist = fromImage.r - fromDrawer.r;
				float greenDist = fromImage.g - fromDrawer.g;
				float blueDist = fromImage.b - fromDrawer.b;


				cumulativeDistance += 0.0001 * (redDist * redDist + greenDist * greenDist + blueDist * blueDist);
			}
		}
	}

	UnloadImageColors(pixels);
	UnloadImage(image);

#ifdef PROFILING_FITNESS_FUNC
	auto endCalculation = std::chrono::high_resolution_clock::now();
	auto drawTime = std::chrono::duration_cast<std::chrono::microseconds>(endDraw - beginDraw);
	auto loadingDataTime = std::chrono::duration_cast<std::chrono::microseconds>(endLoadingData - beginLoadingData);
	auto calculationTime = std::chrono::duration_cast<std::chrono::microseconds>(endCalculation - beginCalculation);
	auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(endCalculation - beginDraw);


	std::cout << "Draw Time: " << drawTime << " Calculation Time: " << calculationTime <<" Loading From GPU: "<< loadingDataTime << " Total Time: " << totalTime<< std::endl;
#endif
	return 1000000/cumulativeDistance;
}

void LineDraw::LineDrawer::LogParameters() const
{
	std::cout << "Fitness: " << fitness << std::endl;
}

void LineDraw::LineDrawer::Draw() const
{
	//for (Vector2 pt : s_lookupTable) {
	//	DrawCircleV(pt, 2, RED);
	//} // Draw Lookup Table

	for (size_t i = 0; i < m_lineIndices.size(); i++) {
		DrawLineEx(s_lookupTable[m_lineIndices[i].ptAIndex], s_lookupTable[m_lineIndices[i].ptBIndex], LINE_WIDTH, m_colors[i]);
	}
}
