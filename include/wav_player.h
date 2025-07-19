#pragma once // Ensure this header file only included once by compiler

#include <string> // For using std::string to handle file paths

// Declares a function that will play a WAV file at the given file path
void play_wav_file(const std::string& filepath);