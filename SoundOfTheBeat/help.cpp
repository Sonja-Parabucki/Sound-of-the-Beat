#include "help.h"


void showHelp(GLFWwindow* window) {
    float wWidth = windowWidth();
    float wHeight = windowHeight();

    std::string title = "HOW TO PLAY?";
    std::string text1 = "Shoot to the sound of the BEAT!";
    std::string text2 = "Nail the timing and earn 2 points and chain hits for massive combos!";
    std::string text3 = "Off-beat shots score zero. Miss a beat? That's -5 points!";
    std::string text4 = "It's game over if your score drops to 0, so best be careful. Also, watch out for bombs!";
    std::string text6 = "EASY MODE: All targets are white. Blast 'em with either mouse button.";
    std::string text7 = "HARD MODE: Red targets = left-click. Green targets = right-click. No mistakes!";
    std::string text8 = "PRESS SPACE TO PAUSE.";
    std::string text9 = "SWITCH TRACKS WITH LEFT/RIGHT ARROW KEYS.";
    std::string exitTx = "[ESC] BACK TO MENU";

    glClearColor(0.0, 0.05, 0.0, 1.0);
    double renderStart;
    while (true) {
        if (glfwWindowShouldClose(window)) break;
        renderStart = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            break;

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

        limitFPS(renderStart);
    }
}