#include "pause.h"

const double FPS = 60.0;
const double FRAME_TIME = 1.0 / FPS;

bool pause(GLFWwindow* window, unsigned int shader) {
    float vertices[] =
    {  //X    Y       R    G    B    A
       -0.06,  0.1,    0.0, 1.0, 0.0, 1.0,
        0.06,  0.0,    0.0, 1.0, 0.0, 1.0,
       -0.06,  -0.1,    0.0, 1.0, 0.0, 1.0
    };
    unsigned int stride = (2 + 4) * sizeof(float);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    

    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //render petlja
    glClearColor(0.0, 0.1, 0.0, 1.0);

    double renderStart, renderTime;
    bool backToMenu = false;
    while (true) {
        renderStart = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            backToMenu = true;
            break;
        }
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            //back to the game
            break;
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();

        //limit FPS
        renderTime = glfwGetTime() - renderStart;
        if (renderTime < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - renderTime));
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
	return backToMenu;
}

