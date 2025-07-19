/**
 * Separate test file for Valgrind testing
 * declares functions from audio_features.cpp
 * 
 * COMPILATION FOR VALGRIND
g++ -g -O0 -Wall \
  -Iinclude -Isrc \
  src/valgrind_test_audio_features.cpp src/fft_stft.cpp \
  -lsndfile -lfftw3 \
  -o audio_features_debug

  // CHECK THIS LINE
  // must be run in root proj dir, dumps compiled file in this directory tho (not in src)


| Command Part                           | Meaning                                             |
| -------------------------------------- | --------------------------------------------------- |
| `-shared -fPIC`                        | Build a shared library (needed for Python modules)  |
| `` `python3 -m pybind11 --includes` `` | Adds `-I/usr/include/python3.x` and `-Ipybind11`    |
| `$(python3-config --extension-suffix)` | Appends `.so` or platform-specific extension suffix |
| `-lfftw3`                              | Link with FFTW                                      |
 *
valgrind --leak-check=full --track-origins=yes --time-stamp=yes -s ./audio_features_debug

VALGRIND RESULT
==00:00:00:08.620 331697== 
==00:00:00:08.620 331697== HEAP SUMMARY:
==00:00:00:08.620 331697==     in use at exit: 52,888 bytes in 617 blocks
==00:00:00:08.620 331697==   total heap usage: 200,766 allocs, 200,149 frees, 80,983,967 bytes allocated
==00:00:00:08.620 331697== 
==00:00:00:08.621 331697== LEAK SUMMARY:
==00:00:00:08.621 331697==    definitely lost: 0 bytes in 0 blocks
==00:00:00:08.621 331697==    indirectly lost: 0 bytes in 0 blocks
==00:00:00:08.621 331697==      possibly lost: 0 bytes in 0 blocks
==00:00:00:08.621 331697==    still reachable: 52,888 bytes in 617 blocks
==00:00:00:08.621 331697==         suppressed: 0 bytes in 0 blocks
==00:00:00:08.621 331697== Reachable blocks (those to which a pointer was found) are not shown.
==00:00:00:08.621 331697== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==00:00:00:08.621 331697== 
==00:00:00:08.621 331697== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

 */

#include <vector>
#include <cmath>
#include <sndfile.h>
#include <iostream>
#include <sndfile.h>
#include <fftw3.h>
#include <complex>
#include <fft_stft.hpp>

// get wave data from selected file and 
std::pair<std::vector<float>, int> get_wav_data(std::string const wav_filename) {
    // convert string soundfile name to char
    const char* wav_filename_char = wav_filename.c_str();
    
    std::vector<float> signal_vector;
    //SF_INFO sfinfo;
    // valgrind fix zero-initialize
    SF_INFO sfinfo = {0}; // zero out all fields
    SNDFILE *file = sf_open(wav_filename_char, SFM_READ, &sfinfo);
    int sample_rate = sfinfo.samplerate;

    if (!file) {
        std::cerr << "Failed to open file\n";
        return std::make_pair(signal_vector, sample_rate);
    }

    std::vector<short> samples(sfinfo.frames * sfinfo.channels);
    sf_readf_short(file, samples.data(), sfinfo.frames);
    sf_close(file);

    // Convert short samples to float in range [-1.0, 1.0]
    signal_vector.reserve(samples.size());
    for (short s : samples) {
        signal_vector.push_back(static_cast<float>(s) / 32768.0f);
    }
    return std::make_pair(signal_vector, sample_rate);
}

// audio feature functions (currently rms and zcr)
float calc_rms(const std::vector<float>& sig) {
    float squares = 0.0;
    int N = sig.size();

    for (int i = 0; i < N; ++i) {
        squares += (sig[i] * sig[i]);
    }
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
    return static_cast<float>(zcr_count) / N;
}

// some bogus main function to make valgrind happy
int main() {
    auto [signal, sample_rate] = get_wav_data("/home/elle/Documents/github_repos/audioFeatureExtraction/data/file_example_WAV_1MG.wav");

    if (signal.empty()) {
        std::cerr << "No audio data.\n";
        return 1;
    }

    std::cout << "Sample rate: " << sample_rate << "\n";
    std::cout << "RMS: " << calc_rms(signal) << "\n";
    std::cout << "ZCR: " << calc_zcr(signal) << "\n";

    std::vector<double> signal_double(signal.begin(), signal.end());
    auto stft = compute_stft(signal_double, 512, 256);
    std::cout << "STFT frames: " << stft.size() << "\n";
    return 0;
}