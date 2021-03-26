
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

class AudioEngine
{
private:
    RtAudio::StreamOptions options;
    RtAudio::StreamParameters oParams;
    RtAudio dac;
    unsigned bufferFrames;
    unsigned sampleRate;

public:
    unsigned channels;
    const unsigned callbackReturnValue = 1;
    double *data = nullptr;
    unsigned frameCounter;
    bool checkCount;
    unsigned nFrames;

    AudioEngine(unsigned nChans = 2, unsigned fs = 44100);
    ~AudioEngine();

    int start();
    void stop();
};