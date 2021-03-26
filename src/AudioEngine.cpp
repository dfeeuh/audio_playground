#include "AudioEngine.hpp"

AudioEngine::AudioEngine(unsigned nChans, unsigned fs)
{
    frameCounter = 0;
    checkCount = false;
    nFrames = 0;
    channels = nChans;
    sampleRate = fs;

    data = (double *)calloc(channels, sizeof(double));

    if (dac.getDeviceCount() < 1)
    {
        std::cout << "\nNo audio devices found!\n";
        exit(1);
    }

    // Let RtAudio print messages to stderr.
    dac.showWarnings(true);

    // Set our stream parameters for output only.
    bufferFrames = 512;

    // Currently selecting a default device
    oParams.deviceId = oParams.deviceId = dac.getDefaultOutputDevice();
    oParams.nChannels = channels;
    oParams.firstChannel = 0;
}

AudioEngine::~AudioEngine()
{
    if (dac.isStreamOpen())
        dac.closeStream();
    if (data != nullptr)
        free(data);
}


static void errorCallback(RtAudioError::Type type, const std::string &errorText)
{
    // This example error handling function does exactly the same thing
    // as the embedded RtAudio::error() function.
    std::cout << "in errorCallback" << std::endl;
    if (type == RtAudioError::WARNING)
        std::cerr << '\n'
                    << errorText << "\n\n";
    else if (type != RtAudioError::WARNING)
        throw(RtAudioError(errorText, type));
}

// Interleaved buffers
static int audio_callback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void *data)
{
    unsigned int i, j;
    MY_TYPE *buffer = (MY_TYPE *)outputBuffer;
    AudioEngine *engine = (AudioEngine *)data;
    double *lastValues = engine->data;

    if (status)
        std::cout << "Stream underflow detected!" << std::endl;

    for (i = 0; i < nBufferFrames; i++)
    {
        for (j = 0; j < engine->channels; j++)
        {
            *buffer++ = (MY_TYPE)(lastValues[j] * SCALE * 0.5);
            lastValues[j] += BASE_RATE * (j + 1 + (j * 0.1));
            if (lastValues[j] >= 1.0)
                lastValues[j] -= 2.0;
        }
    }

    engine->frameCounter += nBufferFrames;
    if (engine->checkCount && (engine->frameCounter >= engine->nFrames))
        return engine->callbackReturnValue;

    return 0;
}

int AudioEngine::start()
{
    try
    {
        dac.openStream(&oParams, 
            NULL, FORMAT, sampleRate, &bufferFrames, 
            &audio_callback, (void *)this, &options, &errorCallback);

        dac.startStream();
    }
    catch (RtAudioError &e)
    {
        e.printMessage();
        return -1;
    }

    return 0;
}

void AudioEngine::stop()
{
    try
    {
        // Stop the stream
        dac.stopStream();
    }
    catch (RtAudioError &e)
    {
        e.printMessage();
    }
}