#include "help.h"


void showHelp(GLFWwindow* window) {

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    float wWidth = windowWidth();
    float wHeight = windowHeight();

    std::string title = "HOW TO PLAY?";
    std::string text1 = "Tap to the beat of the song!";
    std::string text2 = "You get 2 points on a hit in the right moment. The combo can make that number bigger.";
    std::string text3 = "You get no points if you hit the ball too soon or too late. You loose 5 points if you miss a ball.";
    std::string text4 = "It's game over if your score drops to 0, so be careful!";
    std::string text6 = "Easy mode: All balls are white and you destroy them on the left or right mouse-click.";
    std::string text7 = "Hard mode: Red balls are destroyed on the left mouse-click and the blue ball are destroyed on the right mouse-click.";
    std::string text8 = "Click SPACE to pause the game.";
    std::string text9 = "Select the song via the ARROW KEYS LEFT and RIGHT.";
    std::string exitTx = "[ESC] back to MENU";

    //render loop
    glClearColor(0.0, 0.05, 0.0, 1.0);

    double renderStart, renderTime;
    while (true) {
        renderStart = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderText(title, 100, wHeight - 200, 2, 0.93, 0.94, 0.78);

        renderText(text1, 120, wHeight - 400, 0.8, 1., 1., 1.);
        renderText(text2, 120, wHeight - 450, 0.8, 1., 1., 1.);
        renderText(text3, 120, wHeight - 500, 0.8, 1., 1., 1.);
        renderText(text4, 120, wHeight - 550, 0.8, 1., 1., 1.);
        renderText(text6, 120, wHeight - 650, 0.8, 1., 1., 1.);
        renderText(text7, 120, wHeight - 700, 0.8, 1., 1., 1.);
        renderText(text8, 120, wHeight - 800, 0.8, 1., 1., 1.);
        renderText(text9, 120, wHeight - 850, 0.8, 1., 1., 1.);

        renderText(exitTx, 120, 150, 1, 1., 1., 1.);

        glfwSwapBuffers(window);
        glfwPollEvents();

        //limit FPS
        renderTime = glfwGetTime() - renderStart;
        if (renderTime < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - renderTime));
        }
    }
}