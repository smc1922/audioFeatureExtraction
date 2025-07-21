import audio_features
import time
import numpy as np
import matplotlib.pyplot as plt

sample_rate = 48000
hop_size = 512
frame_size = 1024

# Start streaming (e.g., 48000 Hz, 512 frames per buffer)
audio_features.start_streaming(sample_rate, hop_size)

print("Python says: Streaming started...")

print("Python says: Python do nothing while c++ does stuff for 2 seconds")
time.sleep(2)

# buffer = audio_features.get_live_audio_buffer()


# Wait up to 500 ms for buffer to fill
buffer = []
for _ in range(3):
    buffer += audio_features.get_live_audio_buffer()
    print(f"Python Says: Buffer {buffer[:5]}")
    time.sleep(0.5)
    #if buffer:
    #   break


audio_features.stop_streaming()

print("Python says: Streaming stopped.")
print(buffer[:100])  # print first 100 samples

print(f"Python says: Captured {len(buffer)} samples")

# Plot
plt.plot(np.linspace(0, len(buffer)/sample_rate, len(buffer)), buffer)
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.title("Live Audio Buffer")
plt.grid()
plt.show()
