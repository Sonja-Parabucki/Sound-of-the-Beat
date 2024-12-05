#include "pause.h"

const double FPS = 60.0;
const double FRAME_TIME = 1.0 / FPS;

bool pause(GLFWwindow* window, unsigned int shader, int score) {

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    float wWidth = windowWidth();
    float wHeight = windowHeight();

    std::string title = "GAME PAUSED";
    std::string startTx = "press [ENTER] to CONTINUE";
    std::string exitTx = "[ESC] back to MENI";
    std::string scoreTx = "SCORE: " + std::to_string(score);

    //render loop
    glClearColor(0.1, 0.0, 0.0, 1.0);

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


        renderText(title, 100, wHeight - 200, 2);
        renderText(startTx, 120, wHeight / 2, 1);
        renderText(exitTx, 120, wHeight / 2 - 50, 1);
        renderText(scoreTx, 120, 150, 1.2);

        glfwSwapBuffers(window);
        glfwPollEvents();

        //limit FPS
        renderTime = glfwGetTime() - renderStart;
        if (renderTime < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - renderTime));
        }
    }
	return backToMenu;
}

