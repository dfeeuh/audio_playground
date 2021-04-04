#include "AudioEngine.hpp"

AudioEngine::AudioEngine(unsigned nChans, unsigned fs, unsigned framesPerBuf) : 
    frameCounter{0},
    checkCount{false},
    channels{nChans},
    sampleRate{fs},
    bufferFrames{framesPerBuf}, // Set our stream parameters for output only.
    modules{nullptr}
{
    if (rtaudio.getDeviceCount() < 1)
    {
        std::cout << "\nNo audio devices found!\n";
        return;
    }

    // Let RtAudio print messages to stderr.
    rtaudio.showWarnings(true);

    // iParams.deviceId = rtaudio.getDefaultInputDevice();
    // iParams.nChannels = channels;
    // iParams.firstChannel = 0;

    // Currently selecting a default device
    oParams.deviceId = rtaudio.getDefaultOutputDevice();
    oParams.nChannels = channels;
    oParams.firstChannel = 0;
}

AudioEngine::~AudioEngine()
{
    if (rtaudio.isStreamOpen())
        rtaudio.closeStream();
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
    void * /*inputBuffer*/,
    unsigned nBufferFrames,
    double streamTime,
    RtAudioStreamStatus status,
    void *data)
{
    auto *engine = (AudioEngine *)data;

    if (status)
        std::cout << "Stream underflow detected!" << std::endl;
        
    auto module = engine->getModule();

    if (module == nullptr)
    {
        // No modules registered, output zeroes
        std::memset(outputBuffer, 0, sizeof(AUDIO_FORMAT_TYPE) * nBufferFrames * engine->channels);
    }
    else
    {
        // void *outBufferAddress = outputBuffer;

        while (true)
        {
            //module->process((AUDIO_FORMAT_TYPE *)outputBuffer, (AUDIO_FORMAT_TYPE *)inputBuffer, nBufferFrames);
            module->process((AUDIO_FORMAT_TYPE *)outputBuffer, (AUDIO_FORMAT_TYPE *)outputBuffer, nBufferFrames);
            module = module->getNext();

            if (module == nullptr)
                break;

            // Swap buffers here
            //std::swap(outputBuffer, inputBuffer);
        }

        // if (outBufferAddress != outputBuffer)
        //     // Copy the contents of outputBuffer to the actual output buffer.
        //     std::memcpy(outBufferAddress, outputBuffer, sizeof(AUDIO_FORMAT_TYPE)*nBufferFrames*engine->channels);
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
        rtaudio.openStream(&oParams,
                       nullptr, FORMAT, sampleRate, &bufferFrames,
                       &audio_callback, (void *)this, &options, &errorCallback);

        rtaudio.startStream();
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
        rtaudio.stopStream();
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