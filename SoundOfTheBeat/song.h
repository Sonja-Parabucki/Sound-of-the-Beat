#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

int startEngine();
void playSong(const char* filename, bool loop);
void stopSong();
void stopEngine();