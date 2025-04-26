#include "menu.h"
#include "game.h"
#include "help.h"
#include "textUtil.h"
#include <map>

const std::string SONG_FOLDER = "resources/songs/";
const std::string SONG_IMG_FOLDER = "resources/song_images/";
const std::string BEATS_FOLDER = "resources/beats/";
const std::string BOMB_FOLDER = "resources/bombs/";


std::map<std::string, int> loadSongsInfo() {
    std::string path = "resources/songList.txt";
    std::map<std::string, int> songs;
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
    std::cout << "Loading for " << songName << std::endl;
    std::string beat_path = songName + ".txt";
    std::vector<double> beats;
    double beat;
    std::ifstream fin_beat(beat_path);
    if (fin_beat && fin_beat.is_open()) {
        while ((fin_beat >> beat))
            beats.push_back(beat);
        fin_beat.close();;
        std::cout << "Loaded for " << songName << std::endl;
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
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    unsigned int ballShader = createShader("ball.vert", "ball.frag");
    unsigned int texShader = createShader("tex.vert", "tex.frag");
    unsigned int basicTexShader = createShader("basicTex.vert", "basicTex.frag");
    unsigned int lightShader = createShader("light.vert", "light.frag");
    createLetterShader("letter.vert", "letter.frag", wWidth, wHeight);

    Aim aim(basicTexShader);
    Background background(texShader);
    BallModel ballModel(ballShader);
    BombModel bombModel(ballShader);
    Explosion explosion(texShader);
    Lights lights(lightShader);
    Resources resources{ aim, background, ballModel, bombModel, explosion, lights };

    startEngine();
    //game song
    irrklang::ISound* song;
    std::vector<double> beats;

    GameState gameState;
    Game gameInstance;
    gameInstance.mode = 1;
    while (true) {
        if (glfwWindowShouldClose(window)) break;
        gameInstance = menu(window, songs, selectedSongName, gameInstance.mode);
        selectedSongName = gameInstance.selectedSongName;
        if (gameInstance.next == 0) {
            glDeleteProgram(ballShader);
            glDeleteProgram(texShader);
            glDeleteProgram(basicTexShader);
            glDeleteProgram(lightShader);
            deallocateLetterResources();

            stopSongs();
            stopEngine();

            glfwTerminate();
            return 0;
        }
        if (gameInstance.next == 2) {
            showHelp(window);
            continue;
        }
        std::vector<double> beats = loadSong(BEATS_FOLDER + gameInstance.selectedSongName);
        std::vector<double> bombs = loadSong(BOMB_FOLDER + gameInstance.selectedSongName);
        song = playSong((SONG_FOLDER + gameInstance.selectedSongName + ".wav").c_str(), false, true);

        //start new game
        CameraPosition camera = CameraPosition{ glm::vec3(0.0f, 0.2f, Z_LIMIT), -90.0f, -2.0f };
        gameState = GameState{ song, 10, 0, gameInstance.mode, 0, {}, {}, beats, 0, bombs, 0, camera };
        game(window, &gameState, resources, (SONG_IMG_FOLDER + gameInstance.selectedSongName + ".png").c_str());
        stopSong(song);

        std::cout << "SCORE: " << gameState.score << std::endl;
        if (gameState.score > songs[gameInstance.selectedSongName]) {
            songs[gameInstance.selectedSongName] = gameState.score;

            std::ofstream fout("resources/songList.txt");
            for (auto it = songs.begin(); it != songs.end(); it++)
                fout << it->first << '=' << it->second << std::endl;
            fout.close();
        }
    }

}


