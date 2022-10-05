#include "Dithering.h"


std::vector<Color> GetReducedColorPalette(const Texture2D& texture, int numColors){

	Texture2D reducedColorTexture = CreateReducedColorPaletteTexture(texture,numColors);
	Image reducedColorImage = LoadImageFromTexture(reducedColorTexture);
	Color* pixels = LoadImageColors(reducedColorImage);

	std::unordered_map<std::string, Color> colorMap;
	std::vector<Color> result;

	for (int y = 0; y < reducedColorImage.height; y++) {
		for (int x = 0; x < reducedColorImage.width; x++) {
			Color pixel = pixels[x + y * reducedColorImage.width];

			std::string key = std::to_string(pixel.r) + std::to_string(pixel.g) + std::to_string(pixel.b);
			colorMap[key] = pixel;
		}
	}

	for (const auto& pair : colorMap) {
		result.push_back(pair.second);
	}

	UnloadImageColors(pixels);
	UnloadImage(reducedColorImage);
	UnloadTexture(reducedColorTexture);
	return result;
}

Texture2D CreateReducedColorPaletteTexture(const Texture2D& texture , int numColors){
	// Ordered dithering to reduce color palette. From Acerola video

	numColors = PreviousPowerOfTwo(numColors);
	int size = numColors * numColors;
	std::vector<float>thresholdMap = GetThresholdMap(numColors);
	float spread = 0.1f;

	Vector2 resolution = { (float)texture.width,(float)texture.height };

	// Step 1: Load Shader
	Shader shader = LoadShader(0, "shaders/ReduceColorPalette_fs.glsl");

	// Step 2: Create Render Texture
	RenderTexture2D renderTexture = LoadRenderTexture(texture.width,texture.height);

	int thresholdMapLoc = GetShaderLocation(shader, "thresholdMap");
	int sizeLoc = GetShaderLocation(shader, "size");
	int spreadLoc = GetShaderLocation(shader, "spread");
	int resolutionLoc = GetShaderLocation(shader, "resolution");


	//SetShaderValue(shader,thresholdMapLoc,thresholdMap.thresholdMap,SHADER_UNIFORM_FLOAT);
	SetShaderValueV(shader, thresholdMapLoc, thresholdMap.data(), SHADER_UNIFORM_FLOAT, size);
	SetShaderValue(shader, sizeLoc, &size, SHADER_UNIFORM_INT);
	SetShaderValue(shader, spreadLoc, &spread, SHADER_UNIFORM_FLOAT);
	SetShaderValue(shader, resolutionLoc, &resolution, RL_SHADER_UNIFORM_VEC2);



	
	// Step 3: Draw Texture using begin Render Texture and Begin Shader.
	BeginTextureMode(renderTexture);
	BeginShaderMode(shader);

	DrawTexture(texture,0,0,WHITE);

	EndShaderMode();
	EndTextureMode();


	// Step 4: Unload render texture and shader
	UnloadShader(shader);
	rlUnloadFramebuffer(renderTexture.id); // Hopefully no memory leaks here, remove frame buffer hopefully leaving the texture in tact

	// Step 5: Return texture
	return renderTexture.texture;
}

std::vector<float> GetThresholdMap(int numColors) {
	//Math.ceil(Math.log(x) / Math.log(2))
	if (numColors <= 1) {
		throw std::exception("numColors must be greater than or equal to 2 (it will be converted to a power of 2 if it isnt already)");
	}
	int n = PreviousPowerOfTwo(numColors);

	std::vector<float> result(n * n);

	for (int i = 0; i < n * n; i++) {
		int x = i % n;
		int y = (int)std::floor(i / n);
		
		int Mint = GetThresholdMapAtIndex(x, y, n);
		result[i] = (((float)(Mint + 1)) / (n * n)) - 0.5f;
	}

	return result;

}

int GetThresholdMapAtIndex(int i, int j, int n) {
	if (n == 2) {
		static const int nIs2ThresholdMap[4] = { 0,2,3,1 };
		return nIs2ThresholdMap[i + j * n];
	}
	int nextN = PreviousPowerOfTwo(n - 1);
	return GetThresholdMapAtIndex((int)std::floor(i/2), (int)std::floor(j/2), nextN) + GetThresholdMapAtIndex(i % 2, j % 2, nextN) * 4;
}

uint32_t PreviousPowerOfTwo(uint32_t x) {
	// From https://stackoverflow.com/questions/2679815/previous-power-of-2
	if (x == 0) {
		return 0;
	}
	// x--; // Uncomment this, if you want a strictly less than 'x' result.
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x - (x >> 1);
}