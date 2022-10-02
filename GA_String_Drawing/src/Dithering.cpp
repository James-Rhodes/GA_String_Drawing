#include "Dithering.h"


std::vector<Color> GetReducedColorPalette(const Image& image, int numColors){
	return std::vector<Color>();
}

Texture2D CreateReducedColorPaletteTexture(Texture2D texture, int numColors){
	// Ordered dithering to reduce color palette. From Acerola video

	struct ThresholdMap {
		int size = 4;
		//float thresholdMap[4] = {-0.25,0.25,0.5,0.0}; // For storing thresholdmap  
		float thresholdMap[4] = {1.0,1.0,0.5,0.0}; // For storing thresholdmap  

	};
	ThresholdMap thresholdMap;

	// Step 1: Load Shader
	Shader shader = LoadShader(0, "shaders/ReduceColorPalette_fs.glsl");

	// Step 2: Create Render Texture
	RenderTexture2D renderTexture = LoadRenderTexture(texture.width,texture.height);

	int thresholdMapLoc = GetShaderLocation(shader, "thresholdMap");
	int sizeLoc = GetShaderLocation(shader, "size");
	//SetShaderValue(shader,thresholdMapLoc,thresholdMap.thresholdMap,SHADER_UNIFORM_FLOAT);
	SetShaderValueV(shader, thresholdMapLoc, thresholdMap.thresholdMap, SHADER_UNIFORM_FLOAT,thresholdMap.size);

	SetShaderValue(shader, thresholdMapLoc, &thresholdMap.size, SHADER_UNIFORM_INT);

	
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