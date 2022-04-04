/*
* My design followed a lot of test-driven development, so there 
* are quite a few unit tests. Lots of refactoring, etc.
*
* To run, use >./unit_test
*/ 

#define nDEBUG
#if DEBUG
# include <iostream>
#endif

#include "gtest/gtest.h"
#include "AudioEngine.hpp"
#include "Modules.hpp"


class AudioProcessTest : public ::testing::Test
{
private:
    unsigned nChannels = 2;
    unsigned sampleRate = 44100;
    unsigned frameSize = 512;

public:
    std::unique_ptr<AudioEngine> audio;
    std::shared_ptr<SineWaveform> sine;

    void SetUp() {

        sine = std::make_shared<SineWaveform>(nChannels, sampleRate);
        audio = std::make_unique<AudioEngine>(nChannels, sampleRate, frameSize);
        audio->connect(sine);
    }

    void TearDown() {
    }
};


// Sanity check our mechanism for setting up the effects chain
TEST_F (AudioProcessTest, basic)
{
    // Check we have added a single module
    ASSERT_EQ(audio->getModule(), sine);
}

extern void midiCallback(double deltatime, std::vector<unsigned char> *message, void * /*userData*/);

TEST_F (AudioProcessTest, midiFifoIn)
{
    midiCallback(0, nullptr, nullptr);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}