#include "LineDrawer.h"


std::array<Vector2, CIRCLE_RESOLUTION> LineDraw::LineDrawer::s_lookupTable;
RenderTexture LineDraw::intermediateRender;
RenderTexture LineDraw::currentRender;
Texture2D LineDraw::textureToApproximate;
unsigned int LineDraw::computeShaderProgram; // Compute Shader
unsigned int LineDraw::ssboFitnessDetails; // The buffer id that will contain the fitness details.

void LineDraw::LineDrawer::Init()
{
	if (s_lookupTable[0].x == 0 && s_lookupTable[0].y == 0) { 
		// If the look up table hasn't been initialised then initialise it
		std::cout << "Lookup Table Initialised" << std::endl;

		float angleDelta = 2 * PI / CIRCLE_RESOLUTION;

		for (size_t i = 0; i < s_lookupTable.size(); i++) {
			s_lookupTable[i].x = CIRCLE_RADIUS * cos(angleDelta * i) + 0.5f * GetScreenWidth();
			s_lookupTable[i].y = CIRCLE_RADIUS * sin(angleDelta * i) + 0.5f * GetScreenHeight();
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

	auto beginShader = std::chrono::high_resolution_clock::now();
#endif
	LineDraw::FitnessDetails zeroDistance;
	rlUpdateShaderBuffer(LineDraw::ssboFitnessDetails, &zeroDistance, sizeof(FitnessDetails), 0);

	//Set Shader Uniforms (Textures)
	rlEnableShader(LineDraw::computeShaderProgram);

	rlBindImageTexture(LineDraw::intermediateRender.texture.id, 0, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, true);
	rlBindImageTexture(LineDraw::textureToApproximate.id, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, true);
	rlBindShaderBuffer(LineDraw::ssboFitnessDetails, 2);

	rlComputeShaderDispatch(GetScreenWidth() / 16, GetScreenHeight() / 16, 1);
	rlDisableShader();

	LineDraw::FitnessDetails result;
	rlReadShaderBuffer(LineDraw::ssboFitnessDetails, &result, sizeof(FitnessDetails), 0);
#ifdef PROFILING_FITNESS_FUNC
	auto endShader = std::chrono::high_resolution_clock::now();
	auto drawTime = std::chrono::duration_cast<std::chrono::microseconds>(endDraw - beginDraw);
	auto shaderTime = std::chrono::duration_cast<std::chrono::microseconds>(endShader - beginShader);
	auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(endShader - beginDraw);


	std::cout << "Draw Time: " << drawTime <<" Shader Time: "<< shaderTime<< " Total Time: " << totalTime<< std::endl;
#endif
	return std::exp(100000000/(float)result.distance);
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
