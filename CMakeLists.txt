# Minimum CMake version required
cmake_minimum_required(VERSION 3.12)

# Name of the project
# saying project(WavPlayer) and then, project(audio_features) will override, can only define project once
project(WavPlayer)

# Use C++14
set(CMAKE_CXX_STANDARD 14)

# CMake doesn't know where to fine pybind11 since installed via pip w.out the following 
# Get pybind11 location from pip-installed package
# this allows 'find_package(pybind11 REQUIRED)' to work
execute_process(
    COMMAND python3 -m pybind11 --cmakedir
    OUTPUT_VARIABLE pybind11_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(pybind11_DIR ${pybind11_dir})
# find_package(pybind11 REQUIRED)

# Find pybind11, required for Python bindings
find_package(pybind11 REQUIRED)

# Find libsndfile
find_library(SNDFILE_LIBRARY sndfile REQUIRED)

# Find fftw3
find_library(FFTW_LIB fftw3 REQUIRED)

# Find portaudio_lib
find_library(PORTAUDIO_LIB portaudio REQUIRED)


# ===================================== wav_player module =============================================================
# Define a shared library (Python module) named 'wav_player'
add_library(wav_player MODULE
    src/wav_player.cpp
    src/wav_player_pybind.cpp
    src/portaudio_capture.cpp
)

# Include header files for wav_player module 
target_include_directories(wav_player PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link pybind11 to wav_player module
target_link_libraries(wav_player PRIVATE pybind11::module)

# Remove the 'lib' prefix and set the suffix to '.so' (Python expects this format)
set_target_properties(wav_player PROPERTIES PREFIX "" SUFFIX ".so")

# Automatically move wav_player.so to the project root directory after build
add_custom_command(TARGET wav_player POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:wav_player>
            ${CMAKE_SOURCE_DIR}/wav_player.so
    COMMENT "Moving wav_player.so to project root directory"
)

# ===================================== audio_features module ===========================================================
# Define shared library (Python module) named 'audio_features.cpp'
add_library(audio_features MODULE
    src/audio_features.cpp
    src/fft_stft.cpp
    src/portaudio_capture.cpp
)

# no idea what this does different than the block above
# pybind11_add_module(audio_features audio_features.cpp)

# Include header files for audio_features module
target_include_directories(audio_features PRIVATE ${CMAKE_SOURCE_DIR}/include)

# In CMake, each  target link call replaces the previous one, unless you use target_link_libraries() only once or combine all dependencies
# Link pybind11, libsndfile, and fftw-3.3.10 to audio_features
target_link_libraries(audio_features PRIVATE
    pybind11::module
    ${SNDFILE_LIBRARY}
    ${FFTW_LIB}
    ${PORTAUDIO_LIB}
)

# Set the output to be a .so with no 'lib' prefix (required by Python)
set_target_properties(audio_features PROPERTIES PREFIX "" SUFFIX ".so")

# Move the .so file to the project root directory after build
add_custom_command(TARGET audio_features POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:audio_features>
        ${CMAKE_SOURCE_DIR}/audio_features.so
    COMMENT "Moving audio_features.so to project root directory"
)

# FULL BUILD STEPS
# cd /home/elle/Documents/github_repos/audioFeatureExtraction

# rm -rf build/
# mkdir -p build
# cd build
# cmake -DPython_EXECUTABLE=$(which python3) ..
# make
# cd ..

# this replaces the existing .so file as long as the name is the same
# only need to clean build/

# MEMORY LEAK CHECKS USING VALGRIND

# valgrind --leak-check=full --track-origins=yes python3 test_audio_features.py
# valgrind --leak-check=full --track-origins=yes --time-stamp=yes -s ./fft_stft

