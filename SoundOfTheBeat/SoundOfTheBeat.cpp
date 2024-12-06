#include "menu.h"
#include "game.h"
#include "pause.h"
#include "help.h"
#include "textUtil.h"
#include <map>

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

std::map<std::string, int> loadSongsInfo() {
    std::map<std::string, int> songs;
    
    std::string path = SONG_FOLDER + "songList.txt";
    std::string name, line;
    int score;
    
    std::ifstream fin(path);
    if (fin && fin.is_open()) {
        while (std::getline(fin, line)) {
            auto ind = line.find('=');
            name = line.substr(0, ind);
            score = std::stoi(line.substr(ind+1));

            songs.insert(std::pair<std::string, int>(name, score));
        }
        fin.close();
    }
    return songs;
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
    std::map<std::string, int> songs = loadSongsInfo();
    std::string selectedSongName = songs.begin()->first;
    std::cout << selectedSongName << '\n';

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
    std::vector<double> beats;

    GameState gameState;
    while (true) {
        Game gameInstance = menu(window, songs, selectedSongName);
        selectedSongName = gameInstance.selectedSongName;
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
        if (gameInstance.next == 2) {
            showHelp(window);
            continue;
        }
        std::vector<double> beats = loadSong(gameInstance.selectedSongName);
        std::string songPath = SONG_FOLDER + gameInstance.selectedSongName;
        song = playSong((songPath + ".wav").c_str(), false, true);

        //start new game
        gameState = GameState{ 10, 0, gameInstance.mode, 0, {}, 0 };
        while (true) {
            if (game(window, ballShader, rayShader, texShader, gameState, beats, song, (songPath + ".png").c_str()) == 1) {
                if (pause(window, gameState.score))
                    break; //back to menu
            }
            else break; //finished the song till the end
        }

        stopSong(song);

        std::cout << "SCORE: " << gameState.score << std::endl;
        if (gameState.score > songs[gameInstance.selectedSongName]) {
            songs[gameInstance.selectedSongName] = gameState.score;

            std::ofstream fout(SONG_FOLDER + "songList.txt");
            for (auto it = songs.begin(); it != songs.end(); it++)
                fout << it->first << '=' << it->second << std::endl;
            fout.close();
        }
    }

}


