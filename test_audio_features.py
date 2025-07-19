import audio_features
import numpy as np
import matplotlib.pyplot as plt

# safe import of audio_features.so to avoid loading stale file from sys.path
import importlib.util
import sys
import os

so_path = os.path.abspath("audio_features.so")
spec = importlib.util.spec_from_file_location("audio_features", so_path)
audio_features = importlib.util.module_from_spec(spec)
spec.loader.exec_module(audio_features)

# Test with random signal (using std::vector, so must use python list, cannot accept numpy 1D array)
#signal = [0.1, -0.2, 0.3, -0.4, 0.0, 0.5]
# make this go get the 
signal, sample_rate = audio_features.get_wav_data("data/053847_korg-mono-poly-84275.wav")

rms = audio_features.calc_rms(signal)
zcr = audio_features.calc_zcr(signal)

print("RMS:", rms)
print("ZCR:", zcr)

signal_np = np.array(signal)

duration = len(signal) / sample_rate # total seconds
time_np = np.linspace(0, duration, num=len(signal), endpoint=False)

# Assume: signal = np.array([...]), sample_rate = 44100
# Compute RMS for fixed-size frames:
frame_size = 1024
hop_size = 512

# set rollof percent, n mel filters, and n mfcc
rolloff_pct = 99
n_mel_filters = 26
num_mfcc = 13

num_frames = 1 + (len(signal) - frame_size) // hop_size
rms_values = []

for i in range(num_frames):
    start = i * hop_size
    frame = signal[start:start+frame_size]
    rms = audio_features.calc_rms(frame)  # pass list if using pybind11
    rms_values.append(rms)

# Time indices for frames (centered or start)
frame_times = np.arange(num_frames) * hop_size / sample_rate

zcr_values = []
for i in range(num_frames):
    start = i * hop_size
    frame = signal[start:start+frame_size]
    zcr = audio_features.calc_zcr(frame)
    zcr_values.append(zcr)

print("================Start of Amplitude Spectrum==============================")
# Compute amplitude spectrum
stft = audio_features.compute_stft(signal, frame_size, hop_size)
spectrogram = np.array(stft).T  # shape: [freq_bins, time_frames]

print("================Start Spectral Centroid==============================")
# Compute Spectral Centroid
centroids = audio_features.compute_spectral_centroid(stft, sample_rate, frame_size)

print("================Start of Conversion==============================")
# Convert frame indices to time
frame_times = np.arange(len(centroids)) * hop_size / sample_rate

print("================Start of Rolloff==============================")
# rolloff and mfccs
rolloff = audio_features.compute_spectral_rolloff(stft, sample_rate, frame_size, rolloff_pct)

print("================Start of MFCC==============================")
mfccs = audio_features.compute_mfcc(stft, sample_rate, frame_size, 26, 13)

# convert mfccs (python list of lists) to NumPy array
mfccs_np = np.array(mfccs)

# # Plot with subplots (figure 1)
fig, axs = plt.subplots(3, 1, figsize=(12, 8), sharex=True)

axs[0].plot(np.arange(len(signal_np)) / sample_rate, signal_np)
axs[0].set_title("Waveform")
axs[0].set_ylabel("Amplitude")

axs[1].plot(frame_times, rms_values, color='red')
axs[1].set_title("RMS")
axs[1].set_ylabel("RMS")

axs[2].plot(frame_times, zcr_values, color='green')
axs[2].set_title("Zero Crossing Rate")
axs[2].set_ylabel("ZCR")
axs[2].set_xlabel("Time (s)")

plt.tight_layout()

# waveform plot (figure 2)
plt.figure()
plt.plot(time_np,signal_np)
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.title("Waveform")

# amplitude spectogram plot
plt.figure(figsize=(10, 4))
plt.imshow(20 * np.log10(spectrogram + 1e-10), origin='lower', aspect='auto', cmap='inferno')
plt.title("Spectrogram")
plt.xlabel("Time")
plt.ylabel("Frequency Bin")
plt.colorbar(label="dB")
plt.tight_layout()

# spectral centroid plot
plt.figure(figsize=(10, 4))
plt.plot(frame_times, centroids)
plt.title("Spectral Centroid")
plt.xlabel("Time (s)")
plt.ylabel("Frequency (Hz)")
plt.grid(True)
plt.tight_layout()

# centroid, rolloff, and MFCCs plot
fig, axs = plt.subplots(3, 1, figsize=(12, 10), sharex=True)

axs[0].plot(frame_times, centroids, label='Centroid (Hz)')
axs[0].set_ylabel("Hz")
axs[0].legend()

axs[1].plot(frame_times, rolloff, label='Rolloff (Hz)', color='magenta')
axs[1].set_ylabel("Hz")
axs[1].legend()

im = axs[2].imshow(np.array(mfccs).T, origin='lower', aspect='auto',
                   extent=[0, duration, 0, mfccs_np.shape[1]])
axs[2].set_xlabel("Time (s)")
axs[2].set_ylabel("MFCC Coefficient")
fig.colorbar(im, ax=axs[2], label='coeff value')

plt.tight_layout()

# show plots
print("Showing plots")
plt.show()

# # MFCC Placeholder
# # plt.figure()
# # plt.imshow(mfccs, aspect='auto', origin='lower', extent=[0, duration, 0, num_mfcc])
# # plt.title("MFCCs")
# # plt.xlabel("Time (s)")
# # plt.ylabel("MFCC Coefficients")
# # plt.colorbar()
# # plt.show()

# ## pull data directly into the c++ file from the .wav file
# ## python is the starting point, is orchestrator and does visualization

# ## can pull the data into python if it's not going to be realtime (may want to hold the data in python
# ## then access various models/expose the data to different modules that only integrate with python)

# ## Debugging Notes
# # redirect script ouput to txt file (run the following in terminal)
# # python your_script.py > output.txt 2>&1
