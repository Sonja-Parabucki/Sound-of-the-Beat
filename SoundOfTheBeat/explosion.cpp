#include "explosion.h"

Explosion::Explosion(unsigned int shader) {
    float explosion[] =
    {
        -GEN_LIMIT, -GEN_LIMIT, 1.0,  0.0, 0.0,
        -GEN_LIMIT,  GEN_LIMIT, 1.0,  0.0, 1.0,
         GEN_LIMIT, -GEN_LIMIT, 1.0,  1.0, 0.0,
         GEN_LIMIT,  GEN_LIMIT, 1.0,  1.0, 1.0,
    };
    initVABO(explosion, sizeof(explosion), 5 * sizeof(float), &VAO, &VBO, true);
    texture = loadImageToTexture("resources/img/explosion.png");
 
    auto identity = glm::mat4(1.0f);
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "uTex"), 0);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(identity));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uPV"), 1, GL_FALSE, glm::value_ptr(identity));
    glUseProgram(0);
}

Explosion::~Explosion() {
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Explosion::draw(glm::vec3 explosionPos, float explosionInflation) {
    glUseProgram(shader);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);

    if (explosionInflation < 1.0)
        explosionInflation += EXPLOSION_SPEED;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, explosionPos);
    model = glm::scale(model, glm::vec3(explosionInflation));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(model));

    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}