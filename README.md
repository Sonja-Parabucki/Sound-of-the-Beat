# Sound of the Beat
* 2D on branch: main
* 3D on branch: version-3D
### How to add songs for the 2D version?
1. Add the desired song in the SoundOfTheBeat/resources/song (.wav format) (eg. my-song.wav).
2. Install aubio library if needed.
3. In the beat-detector/main.py change the songName to the name of the song file you added in step 1 (without the extension) (my-song). You can also change the fft size to detect more or less beats.
3. Run the python beat-detector/main.py. 
4. (Optional) Add an image in SoundOfTheBeat/resources/song using the same name for the file as before and the png format (my-song.png).
