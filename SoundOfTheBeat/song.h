#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <irrKlang.h>


int startEngine();
irrklang::ISound* playSong(const char* filename, bool loop, bool paused);
void playEffect();
void playMiss();
void playRay();
void resumeSong(irrklang::ISound* sound);
void pauseSong(irrklang::ISound* sound);
void stopSongs();
void stopSong(irrklang::ISound* sound);
void stopEngine();