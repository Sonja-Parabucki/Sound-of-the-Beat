#include "menu.h"

Game menu(GLFWwindow* window, std::map<std::string, int> songs, std::string selectedSongName, int mode) {
    float wWidth = windowWidth();
    float wHeight = windowHeight();

    std::string title = "SOUND OF THE BEAT";
    std::string startTx = "to START";
    std::string helpTx = "ow to play";
    std::string exitTx = "uit";
    std::string mode1Tx = "[1] Easy";
    std::string mode2Tx = "[2] Hard";
    std::string highScoreTx = "HIGH SCORE: ";

    std::map<std::string, int>::iterator iter = songs.begin();
    for (iter = songs.begin(); iter != songs.end(); iter++) {
        if (iter->first == selectedSongName)
            break;
    }

    glClearColor(0., 0., 0.05, 1.0);

    int next = -1;
    double renderStart, renderTime;
    while (next == -1) {
        if (glfwWindowShouldClose(window)) break;
        renderStart = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            next = 0;
        }
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            next = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            next = 2;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            mode = 0;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            mode = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (iter == songs.begin()) iter = songs.end();
            iter--;
            std::this_thread::sleep_for(std::chrono::duration<double>(0.2));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            iter++;
            if (iter == songs.end()) iter = songs.begin();
            std::this_thread::sleep_for(std::chrono::duration<double>(0.2));
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
        renderText(title, 100, wHeight - 200, 2, 0.78, 0.94, 0.93);
        renderText("[ENTER]", 120, wHeight / 2 - 80, 1, 0.78, 0.94, 0.93);
        renderText(startTx, 260, wHeight / 2 - 80, 1, 1., 1., 1.);
        renderText("[H]", 120, wHeight / 2 - 140, 1, 0.78, 0.94, 0.93);
        renderText(helpTx, 172, wHeight / 2 - 140, 1, 1., 1., 1.);
        renderText("[Q]", 120, wHeight / 2 - 200, 1, 0.93, 0.94, 0.78);
        renderText(exitTx, 172, wHeight / 2 - 200, 1, 1., 1., 1.);

        if (mode == 0) {
            renderText(mode1Tx, wWidth - 400, 200, 0.8, 1., 1., 1.);
            renderText(mode2Tx, wWidth - 400, 150, 0.8, 0.5, 0.5, 0.5);
        }
        else {
            renderText(mode1Tx, wWidth - 400, 200, 0.8, 0.5, 0.5, 0.5);
            renderText(mode2Tx, wWidth - 400, 150, 0.8, 1., 1., 1.);
        }

        renderText("[<<] " + iter->first + " [>>]", 120, wHeight / 2 + 100, 1, 0.84, 0.85, 0.94);
        
        renderText(highScoreTx + std::to_string(iter->second), 120, 150, 1.2, 1., 1., 1.);

        glfwSwapBuffers(window);
        glfwPollEvents();
        limitFPS(renderStart);
    }

    return Game{ mode, next, iter->first };
}