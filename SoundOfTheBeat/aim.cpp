#include "aim.h"

Aim::Aim(unsigned int shader) {
    this->shader = shader;
    float dim1 = 0.01;
    float dim2 = 0.045;
    float aim[] = { 0.0f, -dim2, 0.0f,
                    0.0f, -dim1, 0.0f,

                    0.0f, dim2, 0.0f,
                    0.0f, dim1, 0.0f,

                    -dim2, 0.0f, 0.0f,
                    -dim1, 0.0f, 0.0f,

                    dim2, 0.0f, 0.0f,
                    dim1, 0.0f, 0.0f,
    };
    initVABO(aim, sizeof(aim), 3 * sizeof(float), &this->VAO, &this->VBO, true);

    float aspectRatio = (float) windowWidth()/ windowHeight();
    glUseProgram(shader);
    glUniform1f(glGetUniformLocation(shader, "uAspectRatio"), aspectRatio);
    glUseProgram(0);
}

Aim::~Aim() {
    glDeleteBuffers(1, &this->VBO);
    glDeleteVertexArrays(1, &this->VAO);
}

void Aim::draw() {
    glUseProgram(this->shader);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glUniform3f(glGetUniformLocation(this->shader, "uCol"), 0., 0., 0.3);
    glLineWidth(4);
    for (int i = 0; i < 8; i += 2)
        glDrawArrays(GL_LINES, i, 2);

    glUniform3f(glGetUniformLocation(this->shader, "uCol"), 0.85, 0.9, 0.3);
    glLineWidth(8);
    for (int i = 0; i < 8; i += 2)
        glDrawArrays(GL_LINES, i, 2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}