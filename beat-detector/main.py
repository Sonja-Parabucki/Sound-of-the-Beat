import sys
from aubio import tempo, source

win_s = 512                 # fft size
hop_s = win_s // 2          # hop size


folder = '../SoundOfTheBeat/resources/song/'
songName = 'TNT'
filename = folder + songName + '.wav'

samplerate = 0

s = source(filename, samplerate, hop_s)
samplerate = s.samplerate
a_tempo = tempo("default", win_s, hop_s, samplerate)

# tempo detection delay, in samples
# default to 4 blocks delay to catch up with
delay = 4. * hop_s

# list of beats, in samples
beats = []

# total number of frames read
total_frames = 0
while True:
    samples, read = s()
    is_beat = a_tempo(samples)
    if is_beat:
        this_beat = int(total_frames - delay + is_beat[0] * hop_s)
        this_beat /= float(samplerate)
        print("%f" % (this_beat))
        beats.append(this_beat)
    total_frames += read
    if read < hop_s: break


with open(folder + songName + ".txt", "w") as f:
    for t in beats:
        f.write(f"{t}\n")

with open(folder + "songList.txt", "a") as f:
        f.write(f"{songName}\n")

print("Process complete. Results saved to beats.txt")