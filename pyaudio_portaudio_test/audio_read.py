"""PyAudio Example: Play a wave file."""

# Import libraries
import wave
import pyaudio
import os


# Setting up constants for file path (this is modular due to easier future variable changes)
CHUNK = 1024

# Absolute path to wav file instead of relative path assuming where script will be run from
#FILE_LOCATION = '../data/'
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
FILE_NAME = 'file_example_WAV_1MG.wav'
#FILE_PATH = FILE_LOCATION + FILE_NAME
FILE_PATH = os.path.join(SCRIPT_DIR, '..', 'data', FILE_NAME)

# The 'f'before the string is shorthand for 'str.format()' and allows for embed expressions to strings
# '\n' prints new line
print(f"\nAttempting to play {FILE_PATH}.")
print("The ALSA will be unable to find some audio drivers, that is fine for now.\n")

with wave.open(FILE_PATH, 'rb') as wf:
    # Instantiate PyAudio and initialize PortAudio system resources
    p = pyaudio.PyAudio()

    # Open audio stream
    stream = p.open(
        format=p.get_format_from_width(wf.getsampwidth()),
        channels=wf.getnchannels(),
        rate=wf.getframerate(),
        output=True
        )

    # Play samples from the wave file
    # Documentation on the Assignment Expression ':=' linked in google doc
    while len(data := wf.readframes(CHUNK)):
        stream.write(data)

    # Close audio stream
    stream.close()

    # Release PortAudio system resources
    p.terminate()