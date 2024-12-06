#include "song.h"


irrklang::ISoundEngine* engine;
irrklang::ISoundSource* effect;
irrklang::ISoundSource* miss;
irrklang::ISoundSource* ray;

int startEngine() {
     engine = irrklang::createIrrKlangDevice();

    if (!engine)
        return 1; // error starting up the engine
    effect = engine->addSoundSourceFromFile("resources/cursor/effect.wav");
    miss = engine->addSoundSourceFromFile("resources/cursor/miss.wav");
    ray = engine->addSoundSourceFromFile("resources/cursor/ray.wav");
    ray->setDefaultVolume(0.1);
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