#include <iostream>
#include "menu.h"
#include "game.h"
#include "pause.h"


int main()
{
    std::string path = "scores.txt";
    int highScore = 0;
    std::ifstream fin(path);
    if (fin && fin.is_open()) {
        if (!(fin >> highScore))
            highScore = 0;
        fin.close();
    }
    std::cout << highScore << " = HIGH SCORE\n";

    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    const char windowTitle[] = "Sound of the Beat";
    GLFWwindow* window = glfwCreateWindow(windowWidth(), windowHeight(), windowTitle, glfwGetPrimaryMonitor(), NULL);
    if (window == NULL) {
        std::cout << "Failed to create window";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to load GLEW";
        return 3;
    }

    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int ballShader = createShader("ball.vert", "ball.frag");
    unsigned int rayShader = createShader("basic.vert", "ray.frag");
    unsigned int texShader = createShader("tex.vert", "tex.frag");

    GameState gameState;
    while (true) {
        Game gameInstance = menu(window, basicShader);
        if (gameInstance.next == 0) {
            glDeleteProgram(basicShader);
            glDeleteProgram(ballShader);
            glDeleteProgram(rayShader);
            glDeleteProgram(texShader);

            glfwTerminate();
            return 0;
        }
        //start new game
        gameState = GameState{ 10, gameInstance.mode, 0, {} };
        while (true) {
            if (game(window, ballShader, rayShader, texShader, gameState) == 1) {
                if (pause(window, basicShader))
                    break; //back to menu
            }
            else break; //finished the song till the end
        }


        std::cout << "SCORE: " << gameState.score << std::endl;
        if (gameState.score > highScore) {
            highScore = gameState.score;
            std::ofstream fout(path);
            fout << highScore << std::endl;
            fout.close();
        }
    }

}


