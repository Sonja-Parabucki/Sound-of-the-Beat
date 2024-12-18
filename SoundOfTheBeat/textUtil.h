#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H  

#include "setup.h"

const int FONT_SIZE = 48;

int loadTextLib();

void createLetterShader(const char* vertsh, const char* fragsh, float wWidth, float wHeight);

void deallocateLetterResources();

void renderText(std::string text, float x, float y, float scale, float r, float g, float b);