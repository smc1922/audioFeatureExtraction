#include <iostream> // For basic input/output
#include "../include/wav_player.h" // Include custom header to play adio

void playSound(const std::string& filepath)
{
    // Create command string to play audio
    std::string command = "aplay \"" + filepath +"\"";
    int result = std::system(command.c_str());

    // Check if system call failed (non-zero means error)
    if (result !=0) {
        // Print error message to standard error output
        std::cerr << "Failed to play audio file: " << filepath << std::endl;
    }
}

int main() {
    // Play the wav file located in "data/" directory
    // !Make sure "file_example_WAV_1MG.wav" exists in that directory 
    playSound("../data/file_example_WAV_1MG.wav");

    return 0; // Exit program with success code
}