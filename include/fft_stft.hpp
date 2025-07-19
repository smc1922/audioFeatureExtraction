// FFT and STFT cpp header
#pragma once

#include <vector>
#include <complex>

// FFT
std::vector<std::complex<double>> compute_fft(
    const std::vector<double>& input);

// STFT with windowing
std::vector<std::vector<double>> compute_stft(
    const std::vector<double>& signal, 
    int win_len, 
    int hop_len);

// Spectral Centroid
std::vector<double> compute_spectral_centroid(
    const std::vector<std::vector<double>>& spectrogram, 
    int sample_rate, 
    int fft_size);

// Spectral Rolloff
std::vector<double> compute_spectral_rolloff(
    const std::vector<std::vector<double>>& spectrogram,
    int sample_rate, int fft_size,
    double rolloff_pct = 0.99);
//    double rolloff_pct = 0.99);

// MFCCs
std::vector<std::vector<double>> compute_mfcc(
    const std::vector<std::vector<double>>& spectrogram,
    int sample_rate, int fft_size, int num_mel_filters = 26, int num_mfcc=13);
//    int sample_rate, int fft_size, int num_mel_filters = 26, int num_mfcc = 13);