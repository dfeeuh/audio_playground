
#include "RtAudio.h"

/*
typedef char MY_TYPE;
#define FORMAT RTAUDIO_SINT8
#define SCALE  127.0
*/

typedef signed short MY_TYPE;
#define FORMAT RTAUDIO_SINT16
#define SCALE 32767.0

/*
typedef S24 MY_TYPE;
#define FORMAT RTAUDIO_SINT24
#define SCALE  8388607.0

typedef signed long MY_TYPE;
#define FORMAT RTAUDIO_SINT32
#define SCALE  2147483647.0

typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32
#define SCALE  1.0

typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
#define SCALE  1.0
*/

// Platform-dependent sleep routines.
#if defined(WIN32)
#include <windows.h>
#define SLEEP(milliseconds) Sleep((DWORD)milliseconds)
#else // Unix variants
#include <unistd.h>
#define SLEEP(milliseconds) usleep((unsigned long)(milliseconds * 1000.0))
#endif

#define BASE_RATE 0.005
#define TIME 1.0

class AudioModule
{
protected:
    size_t nChans_;
    AudioModule *pNext = nullptr;

public:
    AudioModule(size_t numChannels) { nChans_ = numChannels; }

    virtual void process(MY_TYPE *inputBuffer, MY_TYPE *outputBuffer, size_t numFrames) = 0;

    void connect(AudioModule *p) { pNext = p; }
    AudioModule *getNext() { return pNext; }
};

class SawWaveform : public AudioModule
{
    double *lastValues_;

public:
    SawWaveform(size_t numChannels) : AudioModule{numChannels}
    {
        lastValues_ = (double *)calloc(numChannels, sizeof(double));
    }

    ~SawWaveform() { free(lastValues_); }

    void process(MY_TYPE *outputBuffer, MY_TYPE *inputBuffer, size_t numFrames)
    {
        for (size_t i = 0; i < numFrames; i++)
        {
            for (size_t j = 0; j < nChans_; j++)
            {
                *outputBuffer++ = (MY_TYPE)(lastValues_[j] * SCALE * 0.5);
                lastValues_[j] += BASE_RATE * (j + 1 + (j * 0.1));
                if (lastValues_[j] >= 1.0)
                    lastValues_[j] -= 2.0;
            }
        }

        if (pNext != nullptr)
            pNext->process(outputBuffer, inputBuffer, numFrames);
    }
};

class AudioEngine
{
private:
    RtAudio::StreamOptions options;
    RtAudio::StreamParameters oParams;
    RtAudio dac;
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