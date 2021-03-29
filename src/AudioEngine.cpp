#include "AudioEngine.hpp"

AudioEngine::AudioEngine(unsigned nChans, unsigned fs, unsigned framesPerBuf) : 
    frameCounter{0},
    checkCount{false},
    channels{nChans},
    sampleRate{fs},
    bufferFrames{framesPerBuf}, // Set our stream parameters for output only.
    modules{nullptr}
{
    if (dac.getDeviceCount() < 1)
    {
        std::cout << "\nNo audio devices found!\n";
        return;
    }

    // Let RtAudio print messages to stderr.
    dac.showWarnings(true);

    // Currently selecting a default device
    oParams.deviceId = oParams.deviceId = dac.getDefaultOutputDevice();
    oParams.nChannels = channels;
    oParams.firstChannel = 0;
}

AudioEngine::~AudioEngine()
{
    if (dac.isStreamOpen())
        dac.closeStream();
}

static void errorCallback(
    RtAudioError::Type type,
    const std::string &errorText)
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
static int audio_callback(
    void *outputBuffer,
    void *inputBuffer,
    unsigned nBufferFrames,
    double streamTime,
    RtAudioStreamStatus status,
    void *data)
{
    auto *engine = (AudioEngine *)data;

    if (status)
        std::cout << "Stream underflow detected!" << std::endl;

    if (engine->getModule() == nullptr)
    {
        // No modules registered, output zeroes
        MY_TYPE *buffer = (MY_TYPE *)outputBuffer;
        for (size_t i = 0; i < nBufferFrames * engine->channels; i++)
        {
            *buffer++ = 0;
        }
    }
    else
    {
        engine->getModule()->process((MY_TYPE *)outputBuffer, (MY_TYPE *)inputBuffer, nBufferFrames);
    }

    // This stops the audio thread after a finite number of frames. Not sure
    // it's needed...
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
        dac.stopStream();
    }
    catch (RtAudioError &e)
    {
        e.printMessage();
    }
}

void AudioEngine::connect(AudioModule *pMod)
{
    auto **p=&modules;
    
    while (*p != nullptr) {
        *p = (*p)->getNext();
    }
    
    *p = pMod;
}