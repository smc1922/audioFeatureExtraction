#include "wav_player.h"  // Include the declaration of our function
#include <cstdlib>       // For std::system()
#include <iostream>      // For console output

// This function plays a .wav file using system tools like aplay or afplay
void play_wav_file(const std::string& filepath) {
#if defined(__linux__)
    // Linux: use 'aplay' to play the WAV file
    std::string cmd = "aplay \"" + filepath + "\"";
#elif defined(__APPLE__)
    // macOS: use 'afplay' to play the WAV file
    std::string cmd = "afplay \"" + filepath + "\"";
#elif defined(_WIN32) || defined(_WIN64)
    // Windows: use PowerShell to play the WAV file
    std::string cmd = "powershell -c (New-Object Media.SoundPlayer \"" + filepath + "\").PlaySync();";
#else
    // If none of the OS macros match, we throw an error
    throw std::runtime_error("Unsupported OS");
#endif

    // Print what file is being played
    std::cout << "Playing: " << filepath << std::endl;

    // Execute the system command to play the file
    int result = std::system(cmd.c_str());

    // Check if the command failed (non-zero return)
    if (result != 0) {
        throw std::runtime_error("Failed to play WAV file.");
    }
}