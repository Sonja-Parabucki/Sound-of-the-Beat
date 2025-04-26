#include "background.h"

Background::Background(unsigned int shader) {
	this->shader = shader;
    float background[] =
    {   // X       Y    Z     S    T 
        //logo
        -LIMIT, -LIMIT, 0,  0.0, 0.0,
        -LIMIT,  LIMIT, 0,  0.0, 1.0,
         LIMIT, -LIMIT, 0,  1.0, 0.0,
         LIMIT,  LIMIT, 0,  1.0, 1.0,
         //left side
         -LIMIT, -LIMIT, 0.0,     0.0, 0.0,
         -LIMIT,  LIMIT, 0.0,     0.0, 1.0,
         -LIMIT, -LIMIT, Z_LIMIT, 1.0, 0.0,
         -LIMIT,  LIMIT, Z_LIMIT, 1.0, 1.0,
         //right side
          LIMIT, -LIMIT, 0.0,     0.0, 0.0,
          LIMIT,  LIMIT, 0.0,     0.0, 1.0,
          LIMIT, -LIMIT, Z_LIMIT, 1.0, 0.0,
          LIMIT,  LIMIT, Z_LIMIT, 1.0, 1.0,

    };
    initVABO(background, sizeof(background), 5 * sizeof(float), &this->VAO, &this->VBO, true);

    this->backgroundTexture = loadImageToTexture("resources/img/back.png");
    this->logoTexture = 0;
    auto identity = glm::mat4(1.0f);
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "uTex"), 0);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(identity));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uPV"), 1, GL_FALSE, glm::value_ptr(identity));
    glUseProgram(0);
}


void Background::setLogoTexture(const char* path) {
    this->logoTexture = loadImageToTexture(path);
}

Background::~Background() {
    glDeleteTextures(1, &this->backgroundTexture);
    glDeleteTextures(1, &this->logoTexture);
    glDeleteBuffers(1, &this->VBO);
    glDeleteVertexArrays(1, &this->VAO);
}

void Background::draw(glm::mat4& projectionView) {
    if (logoTexture == 0 || backgroundTexture == 0) return;
    glUseProgram(shader);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uPV"), 1, GL_FALSE, glm::value_ptr(projectionView));

    glBindTexture(GL_TEXTURE_2D, logoTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}