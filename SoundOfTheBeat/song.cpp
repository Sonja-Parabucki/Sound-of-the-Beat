#include "song.h"


irrklang::ISoundEngine* engine;
irrklang::ISoundSource* effect;
irrklang::ISoundSource* miss;
irrklang::ISoundSource* ray;
irrklang::ISoundSource* gameOver;
irrklang::ISoundSource* win;
irrklang::ISoundSource* explosion;

int startEngine() {
     engine = irrklang::createIrrKlangDevice();

    if (!engine)
        return 1; // error starting up the engine
    effect = engine->addSoundSourceFromFile("resources/sounds/effect.wav");
    miss = engine->addSoundSourceFromFile("resources/sounds/miss.wav");
    ray = engine->addSoundSourceFromFile("resources/sounds/ray.wav");
    ray->setDefaultVolume(0.1);
    gameOver = engine->addSoundSourceFromFile("resources/sounds/game-over.wav");
    win = engine->addSoundSourceFromFile("resources/sounds/success.wav");
    explosion = engine->addSoundSourceFromFile("resources/sounds/explosion.wav");
}

irrklang::ISound* playSong(const char* filename, bool loop, bool paused)
{
    return engine->play2D(filename, loop, paused);
}

void playEffect() {
    engine->play2D(effect);
}

void playMiss() {
    engine->play2D(miss);
}

void playRay() {
    engine->play2D(ray);
}

void playGameOver() {
    engine->play2D(gameOver);
}

void playWin() {
    engine->play2D(win);
}

void playExplosion() {
    engine->play2D(explosion);
}

void resumeSong(irrklang::ISound* sound) {
    sound->setIsPaused(false);
}

void pauseSong(irrklang::ISound* sound) {
    sound->setIsPaused(true);
}

void stopSong(irrklang::ISound* sound) {
    sound->stop();
    sound->drop();
}

void stopSongs() {
    engine->stopAllSounds();
}

void stopEngine() {
    engine->drop();
}