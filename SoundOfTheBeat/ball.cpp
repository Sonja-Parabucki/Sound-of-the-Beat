#include "ball.h"

BallModel::BallModel(unsigned int shader)
	            : model("resources/model/ball.obj") {
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

BallModel::~BallModel() {

}

void BallModel::draw(const std::vector<Ball>& balls, glm::vec3 cameraAt, glm::mat4 projectionView, int mode) {
    glUseProgram(shader);
    setColor(shader, 'y');
    glUniform3f(glGetUniformLocation(shader, "uCameraAt"), cameraAt[0], cameraAt[1], cameraAt[2]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uPV"), 1, GL_FALSE, glm::value_ptr(projectionView));

    for (const auto& ball : balls) {
        if (mode == 1) {
            if (ball.red) setColor(shader, 'r');
            else setColor(shader, 'b');
        }
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, ball.pos);
        model = glm::scale(model, glm::vec3(ball.inflation));
        glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(model));
        this->model.Draw(shader);
    }
    glUseProgram(0);
}