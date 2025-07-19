# Import the compiled C++ module
# This is the reason the test.py and wav_player.so are both in the 
# project root directory so they can see each other
import wav_player # imports the built .so file and whatever else you put in that build (using CMake)
    # src/wav_player.cpp
    # src/wav_player_pybind.cpp
    # include/ (all files here)
    # find_package and target_link_libraries tell this to find pybind11 and link it (figure out what linking is)
    # (everything from add_library and target_include_directories in the CMake file)

# Call the function defined in C++ to play a .wav file
# Make sure the path points to a real WAV file
wav_player.play_wav_file("data/file_example_WAV_1MG.wav")


# when you run CMakeLists.txt it creates a .so file (shared object file)