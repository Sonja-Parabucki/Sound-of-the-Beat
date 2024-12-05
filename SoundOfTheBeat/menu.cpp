#include "menu.h"

const double FPS = 60.0;
const double FRAME_TIME = 1.0 / FPS;


Game menu(GLFWwindow* window, unsigned int shader, int highScore, int selectedSongInd, std::vector<std::string> songNames) {

    //irrklang::ISound* theme = playSong("resources/song/theme.wav", true, false);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    float wWidth = windowWidth();
    float wHeight = windowHeight();

    std::string title = "SOUND OF THE BEAT";
    std::string startTx = "[ENTER] to START";
    std::string exitTx = "[Q]uit";
    std::string mode1Tx = "[1] Easy";
    std::string mode2Tx = "[2] Hard";
    std::string highScoreTx = "HIGH SCORE: " + std::to_string(highScore);

    //render petlja
    glClearColor(0.0, 0.0, 0.1, 1.0);

    int next = -1;
    int mode = 1;

    double renderStart, renderTime;
    while (next == -1) {
        renderStart = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            next = 0;
        }
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            next = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            mode = 0;

        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            mode = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            selectedSongInd = (selectedSongInd - 1) % songNames.size();
            std::this_thread::sleep_for(std::chrono::duration<double>(0.1));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            selectedSongInd = (selectedSongInd + 1) % songNames.size();
            std::this_thread::sleep_for(std::chrono::duration<double>(0.1));
        }

        glClear(GL_COLOR_BUFFER_BIT);
      
        renderText(title, 100, wHeight - 200, 2, 0.78, 0.94, 0.93);
        renderText(startTx, 120, wHeight / 2 - 100, 1, 1., 1., 1.);
        renderText(exitTx, 120, wHeight / 2 - 180, 1, 1., 1., 1.);
        if (mode == 0) {
            renderText(mode1Tx, wWidth - 400, 200, 0.8, 0., 0.82, 0.8);
            renderText(mode2Tx, wWidth - 400, 150, 0.8, 0., 0.52, 0.5);
        }
        else {
            renderText(mode1Tx, wWidth - 400, 200, 0.8, 0., 0.52, 0.5);
            renderText(mode2Tx, wWidth - 400, 150, 0.8, 0., 0.82, 0.8);
        }

        renderText("[<<] " + songNames.at(selectedSongInd) + " [>>]", 120, wHeight / 2 + 100, 1, 0.84, 0.85, 0.94);
        
        renderText(highScoreTx, 120, 150, 1.2, 1., 1., 1.);


        glfwSwapBuffers(window);
        glfwPollEvents();

        //limit FPS
        renderTime = glfwGetTime() - renderStart;
        if (renderTime < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - renderTime));
        }
    }

    //stopSong(theme);

    return Game{ mode, next, selectedSongInd };
}