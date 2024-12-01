#include <iostream>
#include "menu.h"
#include "game.h"
#include "pause.h"
#include "textUtil.h"


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
        std::cout << "Failed to load GLFW :(\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    int wWidth = windowWidth();
    int wHeight = windowHeight();
    const char windowTitle[] = "Sound of the Beat";
    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, windowTitle, glfwGetPrimaryMonitor(), NULL);
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

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    int textLibRet = loadTextLib();
    if (textLibRet == 1) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return 4;
    }
    else if (textLibRet == 2) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return 5;
    }

    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int ballShader = createShader("ball.vert", "ball.frag");
    unsigned int rayShader = createShader("basic.vert", "ray.frag");
    unsigned int texShader = createShader("tex.vert", "tex.frag");
    createLetterShader("letter.vert", "letter.frag", wWidth, wHeight);


    GameState gameState;
    while (true) {
        Game gameInstance = menu(window, basicShader, highScore);
        if (gameInstance.next == 0) {
            glDeleteProgram(basicShader);
            glDeleteProgram(ballShader);
            glDeleteProgram(rayShader);
            glDeleteProgram(texShader);
            deallocateLetterResources();

            glfwTerminate();
            return 0;
        }
        //start new game
        gameState = GameState{ 10, 0, gameInstance.mode, 0, {} };
        while (true) {
            if (game(window, ballShader, rayShader, texShader, gameState) == 1) {
                if (pause(window, basicShader, gameState.score))
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


