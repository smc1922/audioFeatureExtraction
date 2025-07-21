#include "portaudio_capture.hpp"
#include <iostream>

// add tiny delay debugging
#include <thread>
#include <chrono>

AudioStreamer::AudioStreamer(int sample_rate, int frames_per_buffer)
    : stream_(nullptr),
      running_(false),
      sample_rate_(sample_rate),
      frames_per_buffer_(frames_per_buffer) {
    Pa_Initialize();
}

AudioStreamer::~AudioStreamer() {
    stop();
    Pa_Terminate();
}

bool AudioStreamer::start() {
    if (running_) return false;

    // safety checks
    PaDeviceIndex dev = Pa_GetDefaultInputDevice();
    if (dev == paNoDevice) {
        std::cerr << "No default input device found!" << std::endl;
        return false;
    }

    const PaDeviceInfo* devInfo = Pa_GetDeviceInfo(22);
    //const PaDeviceInfo* devInfo = Pa_GetDeviceInfo(dev);
    if (!devInfo) {
        std::cerr << "Failed to get device info!" << std::endl;
        return false;
    }

    if (devInfo->maxInputChannels < 1) {
        std::cerr << "Default input device does not support mono input!" << std::endl;
        return false;
    }

        std::cout << "Using input device: " << devInfo->name << " with "
              << devInfo->maxInputChannels << " channels, latency "
              << devInfo->defaultLowInputLatency << " sec\n";
    
    PaStreamParameters inputParams;
    inputParams.device = 22; //dev;
    inputParams.channelCount = 2;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = devInfo->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    PaError err = Pa_OpenStream(
        &stream_,
        &inputParams,
        nullptr,
        sample_rate_,
        frames_per_buffer_,
        paClipOff,
        &AudioStreamer::streamCallback,
        this
    );

    if (err != paNoError) {
        std::cerr << "Failed to open stream: " << Pa_GetErrorText(err) << "\n";
        return false;
    }

    err = Pa_StartStream(stream_);
    if (err != paNoError) {
        std::cerr << "Failed to start stream: " << Pa_GetErrorText(err) << "\n";
        return false;
    }

    running_ = true;
    return true;
}

void AudioStreamer::stop() {
    if (!running_) return;

    Pa_StopStream(stream_);
    Pa_CloseStream(stream_);
    stream_ = nullptr;
    running_ = false;
}

bool AudioStreamer::isRunning() const {
    return running_;
}

int AudioStreamer::streamCallback(const void* inputBuffer, void*, unsigned long framesPerBuffer,
                                  const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData) {
    // input buffer check
    if (inputBuffer == nullptr) {
        // Silence or input underflow; ignore or insert zeros if you want
        return paContinue;
    }
    auto* self = static_cast<AudioStreamer*>(userData);

    // add adefensive check so callback isn't ignored, check stream is running
    if (!self->isRunning()) return paAbort;

    std::cerr << "[Callback] Received audio buffer with " << framesPerBuffer << " frames\n";

    // inside streamCallback
    // std::this_thread::sleep_for(std::chrono::milliseconds(1));

    self->processInput(static_cast<const float*>(inputBuffer), framesPerBuffer);
    return paContinue;
}

void AudioStreamer::processInput(const float* input, size_t frameCount) {
    std::cout << "Processing " << frameCount << " frames\n";
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    for (size_t i = 0; i < frameCount; ++i) {
        float left = input[i * 2];     // Left channel
        float right = input[i * 2 + 1]; // Right channel

        float mono = 0.5f * (left + right); // Convert to mono
        buffer_.push_back(mono);
        if (buffer_.size() > sample_rate_) { // keep only 1 second
            buffer_.erase(buffer_.begin(), buffer_.begin() + (buffer_.size() - sample_rate_));
}
    }
}

std::vector<float> AudioStreamer::getBufferedAudio() {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    std::vector<float> result = buffer_;
    std::cout << "C++ buffer val 1 before clear:" << buffer_[1] << "\n";
    buffer_.clear();  // Clear once fetched
    //std::cout << "C++ buffer val 1 after clear:" << buffer_[1] << "\n";
    return result;
}

void AudioStreamer::flushBuffer() {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    buffer_.clear();
}