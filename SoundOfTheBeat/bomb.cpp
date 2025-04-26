#include "bomb.h"

BombModel::BombModel(unsigned int shader) : model("resources/model/grenade.obj") {
	this->shader = shader;
    glUseProgram(shader);
    setColor(shader, 'w');
    glUniform1f(glGetUniformLocation(shader, "material.shininess"), 32.0f);
    glUniform3f(glGetUniformLocation(shader, "uLightCol"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shader, "uLightPos"), LIMIT, LIMIT, 0.0f);

    auto identity = glm::mat4(1.0f);
    glUniform3f(glGetUniformLocation(shader, "uCameraAt"), 0.0f, 0.0f, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(identity));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uPV"), 1, GL_FALSE, glm::value_ptr(identity));

    glUseProgram(0);
}

void BombModel::draw(const std::vector<Bomb>& bombs, glm::vec3 cameraAt, glm::mat4 projectionView) {
    glUseProgram(shader);
    setColor(shader, 'g');

    glUniform3f(glGetUniformLocation(shader, "uCameraAt"), cameraAt[0], cameraAt[1], cameraAt[2]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uPV"), 1, GL_FALSE, glm::value_ptr(projectionView));

    for (const auto& bomb : bombs) {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, bomb.pos);
        glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(model));
        this->model.Draw(shader);
    }
    glUseProgram(0);
}
