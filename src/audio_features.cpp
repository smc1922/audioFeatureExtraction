/**
 * Implement audio feature functions: RMS and ZCR
 * Use pybind11 to expose them to Python
 * Define the PYBIND11_MODULE(audio_features, m) module (its own python module named audio_features)
 * Build this via CMake as a separate Python extension module like wav_player, this is its own build target
 * 
 * COMPILATION
g++ -Iinclude -Isrc \
  -g -O0 -Wall -shared -fPIC \
  `python3 -m pybind11 --includes` \
  src/audio_features.cpp -o audio_features$(python3-config --extension-suffix) \
  -lfftw3

  // must be run in root proj dir, dumps compiled file in this directory tho (not in src)


| Command Part                           | Meaning                                             |
| -------------------------------------- | --------------------------------------------------- |
| `-shared -fPIC`                        | Build a shared library (needed for Python modules)  |
| `` `python3 -m pybind11 --includes` `` | Adds `-I/usr/include/python3.x` and `-Ipybind11`    |
| `$(python3-config --extension-suffix)` | Appends `.so` or platform-specific extension suffix |
| `-lfftw3`                              | Link with FFTW                                      |


*  VALGRIND
# valgrind --leak-check=full --track-origins=yes --time-stamp=yes -s ./audio_features
 */

#include <pybind11/pybind11.h> // must install Python development packaget and add its include path when compiling (sudo apt install python3-dev)
#include <pybind11/stl.h>   // automatic conversion of std::vector
#include <pybind11/numpy.h> // for upgrade to numpy support (no longer need vector or pybind11/stl.h unless I use std::vector elsewhere)
                            // change all std::vector<float> to py::array_t<float>, use sig.request() and buf.ptr if I want to switch
                            // py::array_t<T> is how pybind11 maps NumPy arrays to C++
#include <vector>
#include <cmath>
#include <sndfile.h>
#include <iostream>
#include <sndfile.h>
#include <fftw3.h>
#include <complex>
#include <fft_stft.hpp>
#include <portaudio_capture.hpp>

//port audio capture
std::unique_ptr<AudioStreamer> g_streamer;

void start_streaming(int sample_rate = 48000, int frames_per_buffer = 512) {
    if (!g_streamer) {
        g_streamer = std::make_unique<AudioStreamer>(sample_rate, frames_per_buffer);
        g_streamer->start();
    }
}

void stop_streaming() {
    if (g_streamer) {
        g_streamer->stop();
        //g_streamer.reset();
    }
}

void clear_buffer() {
    if (g_streamer) {
        g_streamer->flushBuffer();
        g_streamer.reset();
    }
}

std::vector<float> get_live_audio_buffer() {
    if (g_streamer) {
        return g_streamer->getBufferedAudio();
    }
    return {};
}

// get wave data from selected file and 
std::pair<std::vector<float>, int> get_wav_data(std::string const wav_filename) {
    // convert string soundfile name to char
    const char* wav_filename_char = wav_filename.c_str();
    
    std::vector<float> signal_vector;
    SF_INFO sfinfo;
    SNDFILE *file = sf_open(wav_filename_char, SFM_READ, &sfinfo);
    int sample_rate = sfinfo.samplerate;

    if (!file) {
        std::cerr << "Failed to open file\n";
        return std::make_pair(signal_vector, sample_rate);
    }

    std::vector<short> samples(sfinfo.frames * sfinfo.channels);
    sf_readf_short(file, samples.data(), sfinfo.frames);
    sf_close(file);

    // DEBUGGING
    // // Print size of vector
    // std::cout << "Vector Size before cast: " << samples.size() << std::endl;

    // // Print first 10 samples of std::vector<short>
    // for (int i = 0; i < 10 && i < samples.size(); ++i) {
    //     std::cout << "Short Sample[" << i << "] = " << samples[i] << "\n";
    // }

    // Convert short samples to float in range [-1.0, 1.0]
    signal_vector.reserve(samples.size());
    for (short s : samples) {
        signal_vector.push_back(static_cast<float>(s) / 32768.0f);
    }

    // DEBUGGING    
    // // Print first 10 samples of std::vector<float>
    // for (int i = 0; i < 10 && i < samples.size(); ++i) {
    //     std::cout << "Float Sample[" << i << "] = " << samples[i] << "\n";
    // }

    // // Print size of vector
    // std::cout << "Vector Size just before return: " << samples.size() << std::endl;

    return std::make_pair(signal_vector, sample_rate);
}


// audio feature functions (currently rms and zcr)
float calc_rms(const std::vector<float>& sig) {
    float squares = 0.0;
    int N = sig.size();

    for (int i = 0; i < N; ++i) {
        squares += (sig[i] * sig[i]);
    }
    // Print statement for debugging
    // std::cout << sqrt(squares/ static_cast<float>(N)) << std::endl;

    return std::sqrt(squares / static_cast<float>(N));
}

float calc_zcr(const std::vector<float>& sig) {
    int zcr_count = 0;
    int same_sign_count = 1;
    int N = sig.size(); //total number of samples in the audio sig

    for (int i = 1; i < N; ++i) { // start comparison at second sig to compare to first
        int current_sign = (sig[i] > 0) - (sig[i] < 0);
        int prev_sign = (sig[i - same_sign_count] > 0) - (sig[i - same_sign_count] < 0);

        if (current_sign == 0 || current_sign == prev_sign) {
            same_sign_count++;
        } else {
            zcr_count++;
            same_sign_count = 1;
        }
    }
    // Print statement for debugging
    // std::cout << static_cast<float>(zcr_count) / N << std::endl;

    return static_cast<float>(zcr_count) / N;
}

// python module definition
PYBIND11_MODULE(audio_features, m) {
    m.doc() = "Audio feature extraction module (zcr and rms numpy version)";
    m.def("get_wav_data", &get_wav_data, "Read a Wav file and return samples as a float vector and sample rate as an int");
    m.def("calc_rms", &calc_rms, "Calculate Root Mean Square of a 1D NumPy array");
    m.def("calc_zcr", &calc_zcr, "Calculate Zero Crossing Rate of a 1D NumPy array");
    m.def("compute_stft", &compute_stft, "Compute STFT (amplitude spectrum)");
    m.def("compute_spectral_centroid", &compute_spectral_centroid, "Compute spectral centroid from STFT");
    m.def("compute_spectral_rolloff", &compute_spectral_rolloff, "Compute spectral rolloff frequency (Hz) for each frame");
    m.def("compute_mfcc", &compute_mfcc, "Compute MFCCs given spectrogram; returns [n_frames][n_mfcc]");
    m.def("start_streaming", &start_streaming, "Start live audio capture");
    m.def("stop_streaming", &stop_streaming, "Stop live audio capture");
    m.def("get_live_audio_buffer", &get_live_audio_buffer, "Get current live audio buffer");
    m.def("start_streaming", [](int sample_rate, int hop_size) {
        g_streamer = std::make_unique<AudioStreamer>(sample_rate, hop_size);
        if (!g_streamer->start()) throw std::runtime_error("Failed to start stream");
    });
    m.def("stop_streaming", []() {
        g_streamer->stop();
    });
    m.def("get_live_audio_buffer", []() {
        return g_streamer->getBufferedAudio();
    });
    m.def("flush_buffer", []() {
        g_streamer->flushBuffer();
    });
}
