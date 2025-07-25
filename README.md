# Sound of the Beat 3D

### How to change the songs?
1. Add or remove the desired songs in the _SoundOfTheBeat\resources\songs_ (.wav format) (eg. _my-song.wav_).
2. Install _aubio_ library if needed.
3. Run python _beat-detector\main.py_ to process the new songs and remove the old ones from the list of available songs. You can reset all of the files by running the command with reset (eg. _python main.py reset_) but beware that this will also delete your high scores.
4. (Optional) Add an image in _SoundOfTheBeat\resources\song_images_ using the same name for the file as before and the png format (_my-song.png_).
5. You are ready to play.

### Warning for _Visual Studio_
Before running the game for the first time copy the contents of the _dlls_ folder into the _SoundOfTheBeat\x64\Debug_ or _SoundOfTheBeat\x64\Release_ folder once _Visual Studio_ creates them.
