#pragma once
#include "raylib.h"
#include "rlgl.h"
#include <vector>

std::vector<Color> GetReducedColorPalette(const Image& image, int numColors);

Texture2D CreateReducedColorPaletteTexture(Texture2D image, int numColors);