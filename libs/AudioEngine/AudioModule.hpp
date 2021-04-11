#include "AudioFormat.hpp"
#include <memory>

#pragma once

class AudioModule
{
protected:
    unsigned nChans_;
    std::shared_ptr<AudioModule> pNext = nullptr;

public:
    AudioModule(unsigned numChannels) { nChans_ = numChannels; }
    virtual ~AudioModule() {}

    virtual void process(AUDIO_FORMAT_TYPE *inputBuffer, AUDIO_FORMAT_TYPE *outputBuffer, unsigned numFrames) = 0;

    void connect(std::shared_ptr<AudioModule> p) { pNext = p; }
    std::shared_ptr<AudioModule> getNext() { return pNext; }
};