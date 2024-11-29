#include "textUtil.h"
#include <map>



struct Character {
    unsigned int textureID;  // ID handle of the glyph texture
    int sizeX;
    int sizeY;
    int bearingX;
    int bearingY;   // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};


std::map<char, Character> characters;

unsigned int VAO;
unsigned int VBO;
unsigned int letterShader;


void loadChars(FT_Library ft, FT_Face face) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            face->glyph->advance.x
        };
        characters.insert(std::pair<char, Character>(c, character));
    }
}


void initTextVABO() {
    float vertices[24]{}; //6 * (2 + 2)
    initVABO(vertices, sizeof(vertices), 4 * sizeof(float), &VAO, &VBO, false);
}


int loadTextLib() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        return 1;
    
    FT_Face face;
    if (FT_New_Face(ft, "fonts/OCRAEXT.TTF", 0, &face))
        return 2;

    FT_Set_Pixel_Sizes(face, 0, 48);
    loadChars(ft, face);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    initTextVABO();
}


void createLetterShader(const char* vertsh, const char* fragsh, float aspectRatio) {
    letterShader = createShader(vertsh, fragsh);
    glUseProgram(letterShader);
    unsigned uTexLoc = glGetUniformLocation(letterShader, "uTex");
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)
    unsigned int uTexAspectLoc = glGetUniformLocation(letterShader, "uAspect");
    glUniform1f(uTexAspectLoc, aspectRatio);
    glUseProgram(0);
}

void deleteLetterShader() {
    glDeleteProgram(letterShader);
}


void renderText(std::string text, float x, float y, float scale) {

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glUseProgram(letterShader);

    std::string::const_iterator itc;
    for (itc = text.begin(); itc != text.end(); itc++) {
        Character c = characters[*itc];

        float xpos = x + c.bearingX * scale;
        float ypos = y - (c.sizeY - c.bearingY) * scale;

        float w = c.sizeX * scale;
        float h = c.sizeY * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, c.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (c.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}