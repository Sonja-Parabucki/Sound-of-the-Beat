# Sound of the Beat
* 2D on branch: main
* 3D on branch: version-3D
### How to add songs for the 2D version?
1. Add the desired song in _SoundOfTheBeat/resources/song_ (.wav format) (eg. _my-song.wav_).
2. Install _aubio_ library if needed.
3. In the _beat-detector/main.py_ change the _songName_ to the name of the song file you added in step 1 (without the extension) (_my-song_). You can also change the fft size to detect more or less beats.
4. Run _beat-detector/main.py_.
5. (Optional) Add an image in _SoundOfTheBeat/resources/song_ using the same name for the file as before and the png format (_my-song.png_).
6. You are ready to play.
