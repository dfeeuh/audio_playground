
#include "AudioModule.hpp"
#include "AudioFormat.hpp"
#include <RtAudio.h>
#include <RtMidi.h>
#include <boost/lockfree/queue.hpp>

#pragma once

// Platform-dependent sleep routines.
#if defined(WIN32)
#include <windows.h>
#define SLEEP(milliseconds) Sleep((DWORD)milliseconds)
#else // Unix variants
#include <unistd.h>
#define SLEEP(milliseconds) usleep((unsigned long)(milliseconds * 1000.0))
#endif

struct NoteMessage { 
    int midiPitch;
    float amp;
    float freq;
};

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

    std::shared_ptr<AudioModule> modules;

    // RtMidi 
    std::unique_ptr<RtMidiIn> rtmidiin; 

    bool chooseMidiPort();

public:
    unsigned channels;
    const unsigned callbackReturnValue = 1;
    unsigned frameCounter;
    bool checkCount;
    unsigned nFrames;

    boost::lockfree::queue<NoteMessage> queue;

    AudioEngine(unsigned nChans, unsigned fs, unsigned framesPerBuf);
    ~AudioEngine();

    // Connect the first beginning of the signal chain 
    void connect(std::shared_ptr<AudioModule> mod);
    std::shared_ptr<AudioModule> getModule() { return modules; }

    void readFifo();

    int start();
    void stop();
};