#include "menu.h"
#include "game.h"
#include "pause.h"
#include "textUtil.h"

const std::string SONG_FOLDER = "resources/song/";

GLFWcursor* createCursor() {
    int width, height, channels;
    unsigned char* imageData = stbi_load("resources/cursor/point.png", &width, &height, &channels, 4);
    if (!imageData) {
        std::cout << "Error loading cursor image\n";
        return NULL;
    }

    GLFWimage glfwImage;
    glfwImage.width = width;
    glfwImage.height = height;
    glfwImage.pixels = imageData;

    GLFWcursor* cursor = glfwCreateCursor(&glfwImage, width/2, height/2);
    if (!cursor) {
        std::cout << "Error creating cursor from image\n";
    }

    stbi_image_free(imageData);
    return cursor;
}

std::vector<std::string> loadSongNames() {
    std::vector<std::string> songNames;
    std::string path = SONG_FOLDER + "songList.txt";
    std::string name;
    std::ifstream fin(path);
    if (fin && fin.is_open()) {
        while (std::getline(fin, name)) {
            songNames.push_back(name);
            std::cout << "NAME: " << name << std::endl;
        }
        fin.close();
    }
    return songNames;
}

std::vector<double> loadSong(std::string songName) {
    std::cout << "Loading beats for " << songName << std::endl;
    std::string beat_path = SONG_FOLDER + songName + ".txt";
    std::vector<double> beats;
    double beat;
    std::ifstream fin_beat(beat_path);
    if (fin_beat && fin_beat.is_open()) {
        while ((fin_beat >> beat))
            beats.push_back(beat);
        fin_beat.close();;
        std::cout << "Loaded beats for " << songName << std::endl;
    }
    return beats;
}


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

    std::vector<std::string> songNames = loadSongNames();
    int selectedSongInd = 0;


    if (!glfwInit())
    {
        std::cout << "Failed to load GLFW\n";
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

    GLFWcursor* cursor = createCursor();
    if (cursor)
        glfwSetCursor(window, cursor);

    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int ballShader = createShader("ball.vert", "ball.frag");
    unsigned int rayShader = createShader("basic.vert", "ray.frag");
    unsigned int texShader = createShader("tex.vert", "tex.frag");
    createLetterShader("letter.vert", "letter.frag", wWidth, wHeight);

    startEngine();
    //game song
    irrklang::ISound* song;

    std::vector<double> beats = loadSong(songNames.at(selectedSongInd));

    GameState gameState;
    while (true) {
        Game gameInstance = menu(window, basicShader, highScore, selectedSongInd, songNames);
        if (gameInstance.next == 0) {
            glDeleteProgram(basicShader);
            glDeleteProgram(ballShader);
            glDeleteProgram(rayShader);
            glDeleteProgram(texShader);
            deallocateLetterResources();

            stopSongs();
            stopEngine();

            if (cursor)
                glfwDestroyCursor(cursor);

            glfwTerminate();
            return 0;
        }
        selectedSongInd = gameInstance.selectedSongInd;
        std::vector<double> beats = loadSong(songNames.at(selectedSongInd));
        //std::cout << selectedSongInd << '\n';
        std::string songPath = SONG_FOLDER + songNames.at(selectedSongInd) + ".wav";
        song = playSong(songPath.c_str(), false, true);

        //start new game
        gameState = GameState{ 10, 0, gameInstance.mode, 0, {}, 0 };
        while (true) {
            if (game(window, ballShader, rayShader, texShader, gameState, beats, song) == 1) {
                if (pause(window, basicShader, gameState.score))
                    break; //back to menu
            }
            else break; //finished the song till the end
        }

        stopSong(song);

        std::cout << "SCORE: " << gameState.score << std::endl;
        if (gameState.score > highScore) {
            highScore = gameState.score;
            std::ofstream fout(path);
            fout << highScore << std::endl;
            fout.close();
        }
    }

}


