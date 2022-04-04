#include "AudioEngine.hpp"

#include <iomanip>
#include <cmath>
#define LOG_MESSAGE(MSG) (std::cout << MSG)

#define MIDI_NOTE_ON (0x90)
#define MIDI_NOTE_OFF (0x80)

void midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    AudioEngine *audio = static_cast<AudioEngine *>(userData);
    unsigned int nBytes = message->size();
    for (unsigned int i = 0; i < nBytes; i++)
    {
        LOG_MESSAGE("0x" << std::hex <<  (int)message->at(i) << "|");
        if (i==nBytes-1)
            LOG_MESSAGE("\n");
    }

    if (nBytes == 3)
    {
        /* Filter new MIDI messages by note-on, note-off
          only and add them to the fifo message queue*/
        char statusByte = message->at(0);
        if (((statusByte & MIDI_NOTE_ON) == MIDI_NOTE_ON) || ((statusByte & MIDI_NOTE_OFF) == MIDI_NOTE_OFF))
        {
            NoteMessage note = { message->at(1), 0, 0};
            if ((statusByte & MIDI_NOTE_ON) == MIDI_NOTE_ON)
            {
                // Convert Midi pitch to frequency and add to message queue
                note.freq = pow(2, ((float)note.midiPitch - 69.f)/12.f);
                // Convert MIDI velocity to amp
                note.amp = (float)message->at(2) / 127.f;
            }

            // Post to FIFO
            audio->queue.push(note);
        }
        // LOG_MESSAGE("stamp = " << deltatime << std::endl);
    }
}

AudioEngine::AudioEngine(unsigned nChans, unsigned fs, unsigned framesPerBuf) : frameCounter{0},
                                                                                checkCount{false},
                                                                                channels{nChans},
                                                                                sampleRate{fs},
                                                                                bufferFrames{framesPerBuf}, // Set our stream parameters for output only.
                                                                                modules{nullptr},
                                                                                queue{128}
{
    if (rtaudio.getDeviceCount() < 1)
    {
        LOG_MESSAGE("\nNo audio devices found!\n");
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

    // Set up MIDI
    try
    {
        rtmidiin = std::make_unique<RtMidiIn>();

        // Call function to select port.
        if (chooseMidiPort() == false)
        {
            // Need to do something useful in here
            return;
        }

        //

        // Set our callback function.  This should be done immediately after
        // opening the port to avoid having incoming messages written to the
        // queue instead of sent to the callback function.
        rtmidiin->setCallback(&midiCallback);

        // Don't ignore sysex, timing, or active sensing messages.
        rtmidiin->ignoreTypes(false, false, false);
    }
    catch (RtMidiError &error)
    {
        error.printMessage();
    }
}

bool AudioEngine::chooseMidiPort()
{
    // LOG_MESSAGE("\nWould you like to open a virtual input port? [y/N] ";
    std::string keyHit;
    // std::getline(std::cin, keyHit);
    // if (keyHit == "y")
    // {
    //     rtmidiin->openVirtualPort();
    //     return true;
    // }

    std::string portName;
    unsigned int i = 0, nPorts = rtmidiin->getPortCount();
    if (nPorts == 0)
    {
        LOG_MESSAGE("No input ports available!" << std::endl);
        return false;
    }

    if (nPorts > 0)
    {
        for (i = 0; i < nPorts; i++)
        {
            portName = rtmidiin->getPortName(i);
            LOG_MESSAGE("  Input port #" << i << ": " << portName << '\n');
        }

        // Just select port 1 for now
        LOG_MESSAGE("\nOpening " << rtmidiin->getPortName(0) << std::endl);
        rtmidiin->openPort(0);
        return true;
    }

    return false;
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
    LOG_MESSAGE("in errorCallback" << std::endl);
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
        LOG_MESSAGE("Stream underflow detected!" << std::endl);

    auto module = engine->getModule();

    //engine->readFifo();


    if (module == nullptr)
    {
        // No modules registered, output zeroes
        std::memset(outputBuffer, 0, sizeof(AUDIO_FORMAT_TYPE) * nBufferFrames * engine->channels);
    }
    else
    {
        while (true)
        {
            // Since we're generating signal it doesn't matter that the outputBuffer is the input
            // buffer for the first process call
            module->process((AUDIO_FORMAT_TYPE *)outputBuffer, (AUDIO_FORMAT_TYPE *)outputBuffer, nBufferFrames);
            module = module->getNext();

            if (module == nullptr)
                break;
        }

    }

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

void AudioEngine::connect(std::shared_ptr<AudioModule> mod)
{
    modules = mod;
}

void AudioEngine::readFifo()
{
    NoteMessage message;
    while (queue.pop(message))
    {
        // Add message to a preallocated double-linked list

    }
}
