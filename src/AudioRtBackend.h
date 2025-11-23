#pragma once

#include <RtAudio.h>
#include <cstdint>

class DX7Engine;

class AudioRtBackend {
public:
    AudioRtBackend(DX7Engine& engine,
                   unsigned int sampleRate,
                   unsigned int bufferFrames = 256);
    ~AudioRtBackend();

    void start();
    void stop();

private:
    RtAudio audio_;
    DX7Engine& engine_;
    unsigned int sampleRate_;
    unsigned int bufferFrames_;
    bool running_ = false;

    static int audioCallback(void* outputBuffer,
                             void* inputBuffer,
                             unsigned int nFrames,
                             double streamTime,
                             RtAudioStreamStatus status,
                             void* userData);
};
