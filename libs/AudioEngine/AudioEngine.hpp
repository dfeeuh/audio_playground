
#include "AudioModule.hpp"
#include "AudioFormat.hpp"
#include "RtAudio.h"

#pragma once

// Platform-dependent sleep routines.
#if defined(WIN32)
#include <windows.h>
#define SLEEP(milliseconds) Sleep((DWORD)milliseconds)
#else // Unix variants
#include <unistd.h>
#define SLEEP(milliseconds) usleep((unsigned long)(milliseconds * 1000.0))
#endif

class AudioEngine
{
private:
    RtAudio::StreamOptions options;
    RtAudio::StreamParameters oParams;

    // Even though we have no input we need to swap input and output buffers,
    // so they need to exist.
    //RtAudio::StreamParameters iParams;
    RtAudio rtaudio;
    unsigned bufferFrames;
    unsigned sampleRate;

    AudioModule *modules;

public:
    unsigned channels;
    const unsigned callbackReturnValue = 1;
    unsigned frameCounter;
    bool checkCount;
    unsigned nFrames;

    AudioEngine(unsigned nChans, unsigned fs, unsigned framesPerBuf);
    ~AudioEngine();

    void connect(AudioModule *pMod);
    AudioModule *getModule() { return modules; }

    int start();
    void stop();
};