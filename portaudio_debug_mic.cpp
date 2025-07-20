// compile with
// g++ portaudio_debug_mic.cpp -lportaudio -o micDebug

#include <portaudio.h>
#include <iostream>

int main() {
    Pa_Initialize();
    int numDevices = Pa_GetDeviceCount();
    std::cout << "Number of devices: " << numDevices << "\n";

    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo* info = Pa_GetDeviceInfo(i);
        if (info->maxInputChannels > 0) {
            std::cout << "Device #" << i << ": " << info->name
                      << ", max input channels: " << info->maxInputChannels << "\n";
        }
    }
    Pa_Terminate();
    return 0;
}
