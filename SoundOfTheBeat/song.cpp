#include "song.h"

#include <iostream>
#include <irrKlang.h>
using namespace irrklang;

ISoundEngine* engine;

int startEngine() {
     engine = createIrrKlangDevice();

    if (!engine)
        return 1; // error starting up the engine
}

void playSong(const char* filename, bool loop)
{
    // play some sound stream, looped
    engine->play2D(filename, loop);
}

void stopSong() {
    engine->stopAllSounds();
}

void stopEngine() {
    engine->drop(); // delete engine
}