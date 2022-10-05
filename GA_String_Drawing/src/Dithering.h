#pragma once
#include "raylib.h"
#include "rlgl.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>

std::vector<Color> GetReducedColorPalette(const Texture2D& image, int numColors);
std::vector<Color> GetColorPalette(const Texture2D& image);
Texture2D CreateReducedColorPaletteTexture(const Texture2D& image, int numColors);

std::vector<float> GetThresholdMap(int numColors);

int GetThresholdMapAtIndex(int i, int j, int n);

uint32_t PreviousPowerOfTwo(uint32_t x);