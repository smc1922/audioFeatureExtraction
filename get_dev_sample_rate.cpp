// compile with
// g++ -Wall -g get_dev_sample_rate.cpp -lportaudio -o sample

#include <portaudio.h>
#include <iostream>
#include <vector>

int main() {
    Pa_Initialize();

    int deviceIndex = 22;  // Built-in Audio Analog Stereo
    const PaDeviceInfo* info = Pa_GetDeviceInfo(deviceIndex);

    PaStreamParameters inputParams;
    inputParams.device = deviceIndex;
    inputParams.channelCount = 1; // or 2 if stereo
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = info->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    std::cout << "Checking supported sample rates for: " << info->name << "\n";

    std::vector<int> testRates = {8000, 16000, 22050, 32000, 44100, 48000, 96000};

    for (int rate : testRates) {
        PaError err = Pa_IsFormatSupported(&inputParams, nullptr, rate);
        std::cout << rate << " Hz: "
                  << (err == paFormatIsSupported ? "Supported" : "Not Supported") << "\n";
    }

    Pa_Terminate();
    return 0;
}
