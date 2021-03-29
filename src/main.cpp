/******************************************/
/*
  playsaw.cpp
  by Gary P. Scavone, 2006

  This program will output sawtooth waveforms
  of different frequencies on each channel.
*/
/******************************************/
#include "AudioEngine.hpp"
#include <iostream>
#include <cstdlib>


void usage(void)
{
    // Error function in case of incorrect command-line
    // argument specifications
    std::cout << "\nuseage: playsaw N fs <device> <channelOffset> <time>\n";
    std::cout << "    where N = number of channels,\n";
    std::cout << "    fs = the sample rate,\n";
    std::cout << "    device = optional device to use (default = 0),\n";
    std::cout << "    channelOffset = an optional channel offset on the device (default = 0),\n";
    std::cout << "    and time = an optional time duration in seconds (default = no limit).\n\n";
    exit(0);
}


int main(int argc, char *argv[])
{
    unsigned bufferFrames = 0;

    std::cout << "Starting...";

    unsigned nChannels = 2;
    unsigned sampleRate = 44100;
    unsigned frameSize = 512;

    SawWaveform saw(nChannels);
    AudioEngine audio(nChannels, sampleRate, frameSize);

    audio.connect(&saw);

    if (audio.start() != 0)
    {
        std::cout << "Failed to start...";
        return 0;
    }

    char input;
    //std::cout << "Stream latency = " << dac.getStreamLatency() << "\n" << std::endl;
    std::cout << "\nPlaying ... press <enter> to quit (buffer size = " << bufferFrames << ").\n";
    std::cin.get(input);

    audio.stop();
    
    return 0;
}
