// ============== NO LONGER USED ======================

#pragma once // Ensure this header file only included once by compiler
#warning "wrong_header file"

#include <string> // For using std::string to handle file paths
#include <iostream>

// This function plays a .wav audio file using the Linux `aplay` command-line utility.
// @param filepath A string representing the path to the WAV file.
//        Example: "data/file_example_WAV_1MG.wav"
void playSound(const std::string& filepath);