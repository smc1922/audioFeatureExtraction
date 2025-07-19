# audioFeatureExtraction
[comment]: #Objective
Build a cross-platform library for extracting key audio features, supporting both real-time and batch processing, using C++ for core functionality and Python for usability and integration. The primary development environment will be Linux, but the library should support other platforms (e.g., macOS, Windows).
Features
Installation Instructions

# 📁 audioFeatureExtraction
├── 📄 README.md                    # Project overview and setup instructions
├── 📄 .gitignore                   # Git ignored files list
├── 📁 .venv/                       # Python virtual environment
├── 📁 .vscode/                     # VS Code workspace settings
│   ├── 📄 c_pp_properties.json     # IntelliSense, paths, and compiler settings for C/C++
│   └── 📄 settings.json            # Editor and extension settings (formatting, linking, etc)
│   └── 📄 tasks.json               # Build and task runner configs
├── 📁 audio_playback/              # INDEPENDENT Not Used in Project audio playback module (plays .wav files using system command)
│   └── 📄 audio_read.cpp           # Entry point: calls playSound function with a WAV file path
│   └── 📄 wav_player.h             # NO LONGER IN USE, WILL SHOW ERROR MESSAGE IF ACCESSED
├── 📁 src/                         # Source code
│   ├── 📄 wav_player_pybind.cpp    # Exposes the C++ function to Python using Pybind11
│   └── 📄 wav_player.cpp           # Implements WAV playback logic (os-specific)
├── 📁 build/                       # Build code
│   ├── 📁 CMakeFiles/              # 
│   ├── 📄 cmake_install.cmake      # 
│   ├── 📄 CMakeCache.txt           #
│   └── 📄 Makefile                 # 
├── 📁 data/                        # Data files
│   └── 📄 file_example_WAV_1MG.wav # 
├── 📁 include/                     # Include files
│   └── 📄 wav_player.h             # 
├── 📁 test_scripts/                # Test scripts
│   ├── 📁 my_bindings_project/     # BS initial trying to use pybind11
│       ├── 📄 CMakeLists.txt       # BS initial trying to use pybind11
│       └── 📄 example.cpp          # BS initial trying to use pybind11
│   ├── 📄 audioFeatures.cpp        # WTF is this that I started working on *** where I left off
│   ├── 📄 example.cpp              # BS initial trying to use pybind11
│   ├── 📄 example.py               # BS initial trying to use pybind11
│   ├── 📄 hello_world.cpp          # BS 
│   ├── 📄 hello_wrld.py            # BS
│   └── 📄 setup.py                 # BS initial trying to use pybind11
├── 📁 pyaudio_portaudio_test/      # Tests to ensure pyaudio and portaudio function
│   └── 📄 audio_read.py            # Independent python-only file to test pyaudio and portaudio system resources
├── 📁 fftw-3.3.10/                 # 
├── 📄 audio_features.cpp           # MINE
├── 📄 CMakeLists.txt               # 
├── 📄 graphShit.py                 # MINE
├── 📄 test_wav_player.py           # Executes a python script: imports the wav_player.so
├── 📄 test_audio_features.py       # Executes a python script: imports the wav_player.so
└── 📄 notes.txt                    # 

# source contains all of the c++ implementation logic and python bindings
# test_audio_features.py is the entry point to project, orchestrator: tell c++ what file to look at, import audio features from c++, tell c++ file to get audio features, graph