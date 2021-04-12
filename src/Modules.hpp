#include "AudioModule.hpp"
#include <stdlib.h>
#include <atomic>
#include <algorithm>

#pragma once 

#define BASE_RATE 0.005f
#define TIME 1.0f

class Gain : public AudioModule
{
private:
    std::atomic<AUDIO_FORMAT_TYPE> gain_;
    AUDIO_FORMAT_TYPE oldGain_;

public:
    Gain(unsigned numChannels, AUDIO_FORMAT_TYPE g) : 
        AudioModule{numChannels}, 
        gain_{g},
        oldGain_{g}
    { }

    void process(AUDIO_FORMAT_TYPE *outputBuffer, AUDIO_FORMAT_TYPE *inputBuffer, unsigned numFrames)
    {
        (void)inputBuffer;
        
        AUDIO_FORMAT_TYPE increment = gain_.load() - oldGain_;
        if (increment != 0.f)
            increment /= numFrames;

        for (unsigned i = 0; i < numFrames; i++)
        {
            for (unsigned j=0; j<nChans_; j++) {
                *outputBuffer++ = oldGain_ * (*inputBuffer++);
            }

            oldGain_ += increment;
        }

        // Account for any floating point rounding issues
        oldGain_ = gain_.load();
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
protected:
    AUDIO_FORMAT_TYPE *lastValues_;

public:
    SawWaveform(unsigned numChannels) : AudioModule{numChannels}
    {
        lastValues_ = (AUDIO_FORMAT_TYPE *)calloc(numChannels, sizeof(AUDIO_FORMAT_TYPE));
    }

    ~SawWaveform() { free(lastValues_); }

    void process(AUDIO_FORMAT_TYPE *outputBuffer, AUDIO_FORMAT_TYPE *inputBuffer, unsigned numFrames)
    {
        (void)inputBuffer;
        for (unsigned i = 0; i < numFrames; i++)
        {
            for (unsigned j = 0; j < nChans_; j++)
            {
                *outputBuffer++ = (AUDIO_FORMAT_TYPE)(lastValues_[j] * SCALE * 0.5);
                lastValues_[j] += BASE_RATE * (j + 1 + (j * 0.1f));
                if (lastValues_[j] >= 1.f)
                    lastValues_[j] -= 2.f;
            }
        }
    }
};

class SineWaveform : public SawWaveform
{
public:
    SineWaveform(unsigned numChannels) : SawWaveform(numChannels) {}

    void process(AUDIO_FORMAT_TYPE *outputBuffer, AUDIO_FORMAT_TYPE *inputBuffer, unsigned numFrames)
    {
        (void)inputBuffer;
        for (unsigned i = 0; i < numFrames; i++)
        {
            for (unsigned j = 0; j < nChans_; j++)
            {
                *outputBuffer++ = (AUDIO_FORMAT_TYPE)(sin(lastValues_[j] * M_PI) * SCALE * 0.5f);
                lastValues_[j] += BASE_RATE * (j + 1 + (j * 0.1f));
                if (lastValues_[j] >= 1.f)
                    lastValues_[j] -= 2.f;
            }
        }
    }
};