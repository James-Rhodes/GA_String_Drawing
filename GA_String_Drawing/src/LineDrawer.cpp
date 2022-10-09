#include "LineDrawer.h"


std::array<Vector2, CIRCLE_RESOLUTION> LineDraw::LineDrawer::s_lookupTable;
std::vector<Color> LineDraw::LineDrawer::s_colorLookupTable;
int LineDraw::LineDrawer::s_currFitnessIndex = 0;
RenderTexture LineDraw::intermediateRender;
RenderTexture LineDraw::currentRender;
Texture2D LineDraw::textureToApproximate;
unsigned int LineDraw::computeShaderProgram; // Compute Shader
unsigned int LineDraw::ssboFitnessDetails; // The buffer id that will contain the fitness details.
std::vector<LineDraw::LineDrawer>* LineDraw::populationPointer;
int LineDraw::computeShaderCurrentIndexLoc;
bool LineDraw::LineDrawer::s_firstRun;
int LineDraw::LineDrawer::s_maxFitnessCalculatedOn = 0;

void LineDraw::LineDrawer::Init()
{
	PROFILE_FUNC();
	
	if (s_lookupTable[0].x == 0 && s_lookupTable[0].y == 0) { 
		// If the look up table hasn't been initialised then initialise it
		std::cout << "Lookup Table Initialised" << std::endl;

		float angleDelta = 2 * PI / CIRCLE_RESOLUTION;

		for (size_t i = 0; i < s_lookupTable.size(); i++) {
			s_lookupTable[i].x = CIRCLE_RADIUS * cos(angleDelta * i) + 0.5f * GetScreenWidth();
			s_lookupTable[i].y = CIRCLE_RADIUS * sin(angleDelta * i) + 0.5f * GetScreenHeight();
		}
		rlEnableShader(LineDraw::computeShaderProgram);
		LineDraw::computeShaderCurrentIndexLoc = rlGetLocationUniform(LineDraw::computeShaderProgram, "currentIndex");
		rlDisableShader();
		s_colorLookupTable = GetColorPalette(LineDraw::textureToApproximate);
		s_firstRun = true;
	}


	for (size_t i = 0; i < m_lineIndices.size(); i++) {
		// Initialising with random values
		m_lineIndices[i].ptAIndex = GA_Cpp::GetRandomInt(0, CIRCLE_RESOLUTION - 1);
		m_lineIndices[i].ptBIndex = GA_Cpp::GetRandomInt(0, CIRCLE_RESOLUTION - 1);

		m_colorIndices[i] = GA_Cpp::GetRandomInt(0, (int)s_colorLookupTable.size()-1);
	}
}

void LineDraw::LineDrawer::CrossOver(const LineDrawer& parentA, const LineDrawer& parentB)
{
	PROFILE_FUNC();
	//size_t crossOverPoint = m_lineIndices.size() / 2; // Simple midpoint crossover
	double t = parentA.fitness / (parentA.fitness + parentB.fitness);
	size_t crossOverPoint = (size_t)std::lerp(0,m_lineIndices.size(),t);
	for (size_t i = 0; i < m_lineIndices.size(); i++) {
		if (i < crossOverPoint) {
			m_lineIndices[i].ptAIndex = parentA.m_lineIndices[i].ptAIndex;
			m_lineIndices[i].ptBIndex = parentA.m_lineIndices[i].ptBIndex;

			m_colorIndices[i] = parentA.m_colorIndices[i];
		}
		else {
			m_lineIndices[i].ptAIndex = parentB.m_lineIndices[i].ptAIndex;
			m_lineIndices[i].ptBIndex = parentB.m_lineIndices[i].ptBIndex;

			m_colorIndices[i] = parentB.m_colorIndices[i];
		}
	}
	s_firstRun = false;
	s_currFitnessIndex = 0;
}

void LineDraw::LineDrawer::Mutate(float mutationRate)
{
	PROFILE_FUNC();

	for (size_t i = 0; i < m_lineIndices.size(); i++) {

		if (GA_Cpp::GetRandom01() < mutationRate) {
			m_lineIndices[i].ptAIndex = (m_lineIndices[i].ptAIndex + (int)(CIRCLE_RESOLUTION * GA_Cpp::GetRandom01())) % CIRCLE_RESOLUTION;
			m_lineIndices[i].ptBIndex = (m_lineIndices[i].ptBIndex + (int)(CIRCLE_RESOLUTION * GA_Cpp::GetRandom01())) % CIRCLE_RESOLUTION;

			float randNum = 2.0f * ((float)GA_Cpp::GetRandom01() - 0.5f);
			m_colorIndices[i] = (m_colorIndices[i] + (int)(randNum * ((int)s_colorLookupTable.size() - 1))) % s_colorLookupTable.size();
		}
	}
}

double LineDraw::LineDrawer::CalculateFitness()
{
	PROFILE_FUNC();

	BeginTextureMode(LineDraw::intermediateRender);
	ClearBackground(BACKGROUND_COLOR);

	DrawTextureRec(LineDraw::currentRender.texture, { 0,0,(float)LineDraw::currentRender.texture.width,-(float)LineDraw::currentRender.texture.height }, { 0,0 }, WHITE);

	Draw();

	EndTextureMode();

	// If it is the first member then initialise all of the distances to 0
	bool isFirstFitnessCalculated = s_currFitnessIndex == 0;
	if (isFirstFitnessCalculated) {
		LineDraw::FitnessDetails zeroDistance;
		s_maxFitnessCalculatedOn = 0;
		for (int i = 0; i < POPULATION_SIZE; i++) {
			zeroDistance.distances[i] = 0;
			if (!s_firstRun) {
				s_maxFitnessCalculatedOn += (*populationPointer)[i].isElite ? 0:1;
			}
		}
		rlEnableShader(LineDraw::computeShaderProgram);
		rlUpdateShaderBuffer(LineDraw::ssboFitnessDetails, &zeroDistance, sizeof(FitnessDetails), 0);
		rlDisableShader();
	}

	//Set Shader Uniforms (Textures)
	{
		PROFILE_SCOPE("Compute Shader Running");
		rlEnableShader(LineDraw::computeShaderProgram);

		rlSetUniform(computeShaderCurrentIndexLoc,&s_currFitnessIndex,RL_SHADER_UNIFORM_INT,1);
		rlBindImageTexture(LineDraw::intermediateRender.texture.id, 0, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, true);
		rlBindImageTexture(LineDraw::textureToApproximate.id, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, true);
		rlBindShaderBuffer(LineDraw::ssboFitnessDetails, 2);
		rlComputeShaderDispatch(GetScreenWidth() / 16, GetScreenHeight() / 16, 1);
		rlDisableShader();
	}
	s_currFitnessIndex++;

	//If this is the last fitness being calculated then get the data from the buffer
	bool isLastFitnessCalculated = (s_currFitnessIndex == s_maxFitnessCalculatedOn) && !s_firstRun;
	if (isLastFitnessCalculated) {
		LineDraw::FitnessDetails result;
		PROFILE_SCOPE("Reading Shader Buffer");
		rlEnableShader(LineDraw::computeShaderProgram);
		rlReadShaderBuffer(LineDraw::ssboFitnessDetails, &result, sizeof(FitnessDetails), 0);
		//std::cout << "Result: ";
		//for (int i = 0; i < 10; i++) {
		//	std::cout << result.distances[i]<<" , ";
		//}
		//std::cout << std::endl;
		//std::cout << s_maxFitnessCalculatedOn <<" , "<<s_currFitnessIndex << std::endl;
		UpdateAllFitness(result); // Updates all of the fitnesses based on the formula below and the buffer received off the gpu
		rlDisableShader();
	}

	return 1.0;
}

void LineDraw::LineDrawer::LogParameters() const
{
	std::cout << "Fitness: " << fitness << std::endl;
}

void LineDraw::LineDrawer::Draw() const
{
	PROFILE_FUNC();
	//for (Vector2 pt : s_lookupTable) {
	//	DrawCircleV(pt, 2, RED);
	//} // Draw Lookup Table
	for (size_t i = 0; i < m_lineIndices.size(); i++) {

		Vector2 lineBegin = s_lookupTable[m_lineIndices[i].ptAIndex];
		Vector2 lineEnd = s_lookupTable[m_lineIndices[i].ptBIndex];

		Color color = s_colorLookupTable[m_colorIndices[i]];

		DrawLineEx(lineBegin, lineEnd, LINE_WIDTH, color);
	}
}

void LineDraw::LineDrawer::UpdateAllFitness(const FitnessDetails& fitnessDetails) {

	int currIndex = 0;
	if (!s_firstRun) {
		for (LineDrawer& drawer : *LineDraw::populationPointer) {
			if (!drawer.isElite) {

				// Go through the buffer and set the fitnesses of the population 
				unsigned int distance = fitnessDetails.distances[currIndex];
				drawer.fitness = std::exp(100000000 / (float)distance);				

				currIndex++;
			}
		}
	}
}

