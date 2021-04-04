#include "AudioModule.hpp"
#include <stdlib.h>
#include <atomic>
#include <algorithm>

#pragma once 

#define BASE_RATE 0.005
#define TIME 1.0
#define SCALE  1.0

class Gain : public AudioModule
{
private:
    std::atomic<AUDIO_FORMAT_TYPE> gain_;
    AUDIO_FORMAT_TYPE last_gain_;

public:
    Gain(unsigned numChannels, AUDIO_FORMAT_TYPE g) : 
        AudioModule{numChannels}, 
        gain_{g},
        last_gain_{g}
    { }

    void process(AUDIO_FORMAT_TYPE *outputBuffer, AUDIO_FORMAT_TYPE *inputBuffer, unsigned numFrames)
    {
        auto g = gain_.load();
        for (unsigned i = 0; i < nChans_ * numFrames; i++)
        {
            *outputBuffer++ = g * (*inputBuffer++);
        }
    }

    void update(AUDIO_FORMAT_TYPE g)
    {
        g = std::min(g, 1.f);
        g = std::max(g, 0.f);
        gain_.store(g);
    }
};


class SawWaveform : public AudioModule
{
    AUDIO_FORMAT_TYPE *lastValues_;

public:
    SawWaveform(unsigned numChannels) : AudioModule{numChannels}
    {
        lastValues_ = (AUDIO_FORMAT_TYPE *)calloc(numChannels, sizeof(AUDIO_FORMAT_TYPE));
    }

    ~SawWaveform() { free(lastValues_); }

    void process(AUDIO_FORMAT_TYPE *outputBuffer, AUDIO_FORMAT_TYPE *inputBuffer, unsigned numFrames)
    {
        for (unsigned i = 0; i < numFrames; i++)
        {
            for (unsigned j = 0; j < nChans_; j++)
            {
                *outputBuffer++ = (AUDIO_FORMAT_TYPE)(lastValues_[j] * SCALE * 0.5);
                lastValues_[j] += BASE_RATE * (j + 1 + (j * 0.1));
                if (lastValues_[j] >= 1.0)
                    lastValues_[j] -= 2.0;
            }
        }
    }
};