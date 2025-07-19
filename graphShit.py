import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf # soundfile is angry
# import [my pybind11 module] 

frame_size = 4096 # do something more intelligent here
# hop_size = frame_size * ( 1 - overlap )
hop_size = int(0.25 * frame_size) # prioritize smoother, more accurate feature tracking

# for speech do something like
# frame_size = 0.025 * sample_rate // 25 ms if 16kHz
# hop_size = 0.010 * sample_rate // 10 ms hop if 16kHz