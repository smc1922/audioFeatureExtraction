#pragma once
#include <vector>
#include <mutex>
#include <atomic>
#include <portaudio.h>

class AudioStreamer {
public:
    AudioStreamer(int sample_rate = 48000, int frames_per_buffer = 512);
    ~AudioStreamer();

    bool start();
    void stop();
    bool isRunning() const; 
    void flushBuffer();

    std::vector<float> getBufferedAudio();  // pull a chunk

private:
    static int streamCallback(const void* inputBuffer, void* outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void* userData);

    void processInput(const float* input, size_t frameCount);

    PaStream* stream_;
    std::vector<float> buffer_;
    std::mutex buffer_mutex_;
    std::atomic<bool> running_;
    int sample_rate_;
    int frames_per_buffer_;
};
