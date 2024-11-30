#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H  

#include "setup.h"

int loadTextLib();

void createLetterShader(const char* vertsh, const char* fragsh, float wWidth, float wHeight);

void deleteLetterShader();

//void loadChars(FT_Library ft, FT_Face face);

void renderText(std::string text, float x, float y, float scale);