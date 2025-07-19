// FFT and STFT implementation

#include <fftw3.h>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>

// FFT
std::vector<std::complex<double>> compute_fft(const std::vector<double>& input) {
    int N = input.size();
    fftw_complex* out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    double* in = (double*) fftw_malloc(sizeof(double) * N);

    std::copy(input.begin(), input.end(), in);

    fftw_plan plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
    fftw_execute(plan);

    std::vector<std::complex<double>> result(N / 2 + 1);
    for (int i = 0; i < N / 2 + 1; ++i)
        result[i] = std::complex<double>(out[i][0], out[i][1]);

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return result;
}

// STFT with windowing
std::vector<std::vector<double>> compute_stft(const std::vector<double>& signal, int win_len, int hop_len) {
    int num_frames = (signal.size() - win_len) / hop_len + 1;

    // Hann window
    std::vector<double> window(win_len);
    for (int i = 0; i < win_len; ++i)
        window[i] = 0.5 * (1 - std::cos(2 * M_PI * i / (win_len - 1)));

    std::vector<std::vector<double>> spectrogram;

    for (int frame = 0; frame < num_frames; ++frame) {
        std::vector<double> segment(win_len);
        for (int i = 0; i < win_len; ++i)
            segment[i] = signal[frame * hop_len + i] * window[i];

        auto fft_result = compute_fft(segment);

        std::vector<double> magnitude(fft_result.size());
        for (size_t k = 0; k < fft_result.size(); ++k)
            magnitude[k] = std::abs(fft_result[k]);

        spectrogram.push_back(std::move(magnitude));
    }

    return spectrogram;
}

// Spectral Centroid
std::vector<double> compute_spectral_centroid(const std::vector<std::vector<double>>& spectrogram, int sample_rate, int fft_size) {
    std::vector<double> centroids;

    double bin_hz = static_cast<double>(sample_rate) / fft_size;

    for (const auto& frame : spectrogram) {
        double weighted_sum = 0.0;
        double magnitude_sum = 0.0;

        for (size_t k = 0; k < frame.size(); ++k) {
            weighted_sum += k * bin_hz * frame[k];  // bin index * frequency * magnitude
            magnitude_sum += frame[k];
        }

        if (magnitude_sum > 1e-6)
            centroids.push_back(weighted_sum / magnitude_sum);
        else
            centroids.push_back(0.0);
    }

    return centroids;
}

// Spectral Rolloff (frequency below which 99 percent of spectral energy is contained)
std::vector<double> compute_spectral_rolloff(
    const std::vector<std::vector<double>>& spectrogram,
    int sample_rate, int fft_size, double rolloff_pct) {

    int bins = spectrogram[0].size();
    double bin_hz = static_cast<double>(sample_rate) / fft_size;
    std::vector<double> rolloffs;
    rolloffs.reserve(spectrogram.size());

    for (const auto& frame : spectrogram) {
        double total_energy = 0;
        for (double mag : frame) total_energy += mag;
        double threshold = rolloff_pct * total_energy;

        double cumulative = 0;
        int roll_bin = bins - 1;
        for (int k = 0; k < bins; ++k) {
            cumulative += frame[k];
            if (cumulative >= threshold) { roll_bin = k; break; }
        }
        rolloffs.push_back(roll_bin * bin_hz);
    }
    return rolloffs;
}

// MFCCs

// Helpers: mel/freq conversion
double hz_to_mel(double hz) { return 2595 * std::log10(1 + hz / 700.0); }
double mel_to_hz(double mel) { return 700 * (std::pow(10, mel / 2595.0) - 1); }

std::vector<std::vector<double>> compute_mfcc(
    const std::vector<std::vector<double>>& spectrogram,
    int sample_rate, int fft_size, int n_mel=26, int n_mfcc=13) {

    int n_frames = spectrogram.size();
    int n_bins = fft_size / 2 + 1;
    double max_mel = hz_to_mel(sample_rate / 2.0);
    double min_mel = hz_to_mel(0.0);
    std::vector<double> mel_points(n_mel + 2);

    for (int i = 0; i < (int)mel_points.size(); ++i)
        mel_points[i] = mel_to_hz(min_mel + (max_mel - min_mel) * i / (n_mel + 1));

    std::vector<int> bin_indices(mel_points.size());
    for (size_t i = 0; i < mel_points.size(); ++i)
        bin_indices[i] = static_cast<int>(std::floor((fft_size + 1) * mel_points[i] / sample_rate));

    std::vector<std::vector<double>> mel_filterbank(n_mel, std::vector<double>(n_bins, 0.0));
    for (int m = 1; m <= n_mel; ++m) {
        int f_m_minus = bin_indices[m - 1];
        int f_m = bin_indices[m];
        int f_m_plus = bin_indices[m + 1];
        for (int k = f_m_minus; k < f_m; ++k)
            mel_filterbank[m - 1][k] = (k - f_m_minus) / double(f_m - f_m_minus);
        for (int k = f_m; k < f_m_plus; ++k)
            mel_filterbank[m - 1][k] = (f_m_plus - k) / double(f_m_plus - f_m);
    }

    std::vector<std::vector<double>> mfccs(n_frames, std::vector<double>(n_mfcc));
    for (int t = 0; t < n_frames; ++t) {
        std::vector<double> mel_energies(n_mel, 0.0);
        for (int m = 0; m < n_mel; ++m)
            for (int k = 0; k < n_bins; ++k)
                mel_energies[m] += spectrogram[t][k] * mel_filterbank[m][k];
        for (auto& e : mel_energies) e = std::log(e + 1e-10);

        for (int i = 0; i < n_mfcc; ++i) {
            double sum = 0;
            for (int m = 0; m < n_mel; ++m)
                sum += mel_energies[m] * std::cos(M_PI * i * (m + 0.5) / n_mel);
            mfccs[t][i] = sum;
        }
    }
    return mfccs;
}

// add main function to use valgrind
// int main() {
//     std::vector<double> signal = {0.0, 1.0, 0.0, -1.0};  // simple test signal

//     auto fft_result = compute_fft(signal);

//     std::cout << "FFT Result:\n";
//     for (const auto& c : fft_result)
//         std::cout << c << "\n";

//     return 0;
// }

// compile c++ files to check for memory leaks
// g++ -g -o fft_stft fft_stft.cpp

// run valgrind to find memory leaks
// # valgrind --leak-check=full --track-origins=yes --time-stamp=yes -s ./fft_stft
/**
 * ==00:00:00:00.951 286830== 
==00:00:00:00.951 286830== HEAP SUMMARY:
==00:00:00:00.951 286830==     in use at exit: 52,768 bytes in 617 blocks
==00:00:00:00.951 286830==   total heap usage: 661 allocs, 44 frees, 216,252 bytes allocated
==00:00:00:00.951 286830== 
==00:00:00:00.952 286830== LEAK SUMMARY:
==00:00:00:00.952 286830==    definitely lost: 0 bytes in 0 blocks
==00:00:00:00.952 286830==    indirectly lost: 0 bytes in 0 blocks
==00:00:00:00.952 286830==      possibly lost: 0 bytes in 0 blocks
==00:00:00:00.952 286830==    still reachable: 52,768 bytes in 617 blocks
==00:00:00:00.952 286830==         suppressed: 0 bytes in 0 blocks
==00:00:00:00.952 286830== Reachable blocks (those to which a pointer was found) are not shown.
==00:00:00:00.952 286830== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==00:00:00:00.952 286830== 
==00:00:00:00.952 286830== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
 */