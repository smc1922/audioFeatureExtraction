#include <pybind11/pybind11.h> // pybind11 core header
#include "wav_player.h"        // Include our WAV playback function

namespace py = pybind11; // Create a shortcut so we don't have to write pybind11:: every time

// This macro creates a Python module named 'wav_player'
PYBIND11_MODULE(wav_player, m) {
    // We add the C++ function 'play_wav_file' to the Python module
    m.def("play_wav_file", &play_wav_file, "Play a WAV file from the given file path");
}