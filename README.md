# Serial communication
The pi that controls the notes to be played and sends them over the serial link will be called rpi1.

The pi that makes the frives play the notes will be rpi2.

rpi1 will need to send every note needed to rpi2 and rpi2 will need to handle them on time.

**Information that needs to be sent:**

* Note - 12 possible - 4 bits
* Octave - Up to 4 - 2 bits
* Which frive - Up to 8 - 3 bits

# Notes
| Note | Frequency(Hz) | Duration(µs) |
|----|----|----|
| A#2/Bb2  | 116.54  | 8581 |
| B2 | 123.47  | 8099 |
| C3 | 130.81  | 7645 |
| C#3/Db3  | 138.59  | 7216 |
| D3 | 146.83  | 6811 |
| D#3/Eb3  | 155.56  | 6428 |
| E3 | 164.81  | 6068 |
| F3 | 174.61  | 5727 |
| F#3/Gb3  | 185.00  | 5405 |
| G3 | 196.00  | 5102 |
| G#3/Ab3  | 207.65  | 4816 |
| A3 | 220.00  | 4545 |
| A#3/Bb3  | 233.08  | 4290 |
| B3 | 246.94  | 4050 |
| C4 | 261.63  | 3822 |
| C#4/Db4  | 277.18  | 3608 |
| D4 | 293.66  | 3405 |
| D#4/Eb4  | 311.13  | 3214 |
| E4 | 329.63  | 3034 |
| F4 | 349.23  | 2863 |
| F#4/Gb4  | 369.99  | 2703 |
| G4 | 392.00  | 2551 |
| G#4/Ab4  | 415.30  | 2408 |
| A4  | 440.00 | 2273 |
| A#4/Bb4  | 466.16 | 2145 |
| B4  | 493.88 | 2025 |
| C5  | 523.25 | 1911 |
| C#5/Db5  | 554.37 | 1820 |
| D5  | 587.33 | 1703 |
| D#5/Eb5  | 622.25 | 1607 |
| E5  | 659.25 | 1517 |
| F5  | 698.46 | 1432 |
| F#5/Gb5  | 739.99 | 1351 |
| G5  | 783.99 | 1276 |
| G#5/Ab5  | 830.61 | 1204 |
| A5  | 880.00 | 1136 |
| A#5/Bb5  | 932.33 | 1073 |
| B5  | 987.77 | 1012 |
| C6  | 1046.50 | 956 |
| C#6/Db6  | 1108.73 | 902 |
| D6  | 1174.66 | 851 |


# Ideas
Using PWM: https://www.raspberrypi.org/forums/viewtopic.php?f=63&t=113916<br>
Using wiringPi: http://wiringpi.com/reference/core-functions/
