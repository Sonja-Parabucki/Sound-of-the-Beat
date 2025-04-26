#include "lights.h"

Lights::Lights(unsigned int shader) 
		: modelZ("resources/model/tube.obj"), 
		  modelX("resources/model/tubeH.obj") {
	this->shader = shader;

	auto identity = glm::mat4(1.0f);
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "uPV"), 1, GL_FALSE, glm::value_ptr(identity));
	glUseProgram(0);
}

void Lights::draw(glm::mat4 projectionView) {
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uPV"), 1, GL_FALSE, glm::value_ptr(projectionView));

    glm::mat4 modelLight;
    float lightPosX = LIMIT;
    float lightPosY = LIMIT;
    for (int i = 0; i < 2; i++, lightPosX = -lightPosX) {
        for (int j = 0; j < 2; j++, lightPosY = -lightPosY) {
            modelLight = glm::mat4(1.0f);
            modelLight = glm::translate(modelLight, glm::vec3(lightPosX, lightPosY, 0.0f));
            modelLight = glm::rotate(modelLight, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(modelLight));
            modelZ.Draw(shader);
        }
        modelLight = glm::mat4(1.0f);
        modelLight = glm::translate(modelLight, glm::vec3(LIMIT, lightPosX, 0.0f));
        modelLight = glm::rotate(modelLight, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(modelLight));
        modelX.Draw(shader);
    }
    glUseProgram(0);
}