import sys
from pathlib import Path
from aubio import tempo, source
from random import randint

res_folder = '../SoundOfTheBeat/resources/'
songs_folder = res_folder + 'songs/'
# songs_folder = res_folder + 'test/'
beats_folder = res_folder + 'beats/'
bombs_folder = res_folder + 'bombs/'

def process(path, song_name, fft_size):
    win_s = fft_size
    hop_s = win_s // 2          # hop size
    samplerate = 0

    s = source(path, samplerate, hop_s)
    samplerate = s.samplerate
    a_tempo = tempo("default", win_s, hop_s, samplerate)

    # tempo detection delay, in samples
    # default to 4 blocks delay to catch up with
    delay = 4. * hop_s

    # list of beats, in samples
    beats = []
    bombs = []

    # total number of frames read
    total_frames = 0
    while True:
        samples, read = s()
        is_beat = a_tempo(samples)
        if is_beat:
            this_beat = int(total_frames - delay + is_beat[0] * hop_s)
            this_beat /= float(samplerate)
            # print("%f" % (this_beat))
            beats.append(this_beat)

            if randint(1, 20) > 18:
                bombs.append(this_beat)

        total_frames += read
        if read < hop_s: break

    with open(beats_folder + song_name + ".txt", "w") as f:
        for t in beats:
            f.write(f"{t}\n")

    with open(bombs_folder + song_name + ".txt", "w") as f:
        for t in bombs:
            f.write(f"{t}\n")


if (__name__ == "__main__"):
    print("Processing...")
    try:
        reset_all = sys.argv[1] == 'reset'
    except:
        reset_all = False

    songs = {}
    with open(res_folder + "songList.txt", "r") as f:
        for line in f:
            name, score = line.split('=')
            songs[name] = score[:-1]    #remove '\n'
        print(songs.items())

    song_files = []
    path = Path(songs_folder)
    for file in path.iterdir():
        if file.is_file():
            name = file.name[:-4]
            song_files.append(name)
            if (not reset_all and name in songs):
                continue
            filename = file.__str__()

            print(f'Updating {name}...')
            process(filename, name, 1024)
            songs[name] = '0'

    with open(res_folder + "songList.txt", "w") as f:
        for name, score in sorted(songs.items()):
            if (name in song_files):
                f.write(f"{name}={songs[name]}\n")
    print("...done")
            