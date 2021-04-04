#include "AudioFormat.hpp"

#pragma once

class AudioModule
{
protected:
    unsigned nChans_;
    AudioModule *pNext = nullptr;

public:
    AudioModule(unsigned numChannels) { nChans_ = numChannels; }

    virtual void process(AUDIO_FORMAT_TYPE *inputBuffer, AUDIO_FORMAT_TYPE *outputBuffer, unsigned numFrames) = 0;

    void connect(AudioModule *p) { pNext = p; }
    AudioModule *getNext() { return pNext; }
};