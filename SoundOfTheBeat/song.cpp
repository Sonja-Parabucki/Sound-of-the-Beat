#include "song.h"


irrklang::ISoundEngine* engine;

int startEngine() {
     engine = irrklang::createIrrKlangDevice();

    if (!engine)
        return 1; // error starting up the engine
}

irrklang::ISound* playSong(const char* filename, bool loop, bool paused)
{
    // play some sound stream, looped
    return engine->play2D(filename, loop, paused);
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
    engine->drop(); // delete engine
}