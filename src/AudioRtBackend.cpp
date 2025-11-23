#include "AudioRtBackend.h"
#include "DX7Engine.h"

#include <iostream>
#include <stdexcept>

AudioRtBackend::AudioRtBackend(DX7Engine& engine,
                               unsigned int sampleRate,
                               unsigned int bufferFrames)
    : audio_(),
      engine_(engine),
      sampleRate_(sampleRate),
      bufferFrames_(bufferFrames)
{
}

AudioRtBackend::~AudioRtBackend() {
    try {
        stop();
    } catch (...) {
        // ignore during destruction
    }
}

void AudioRtBackend::start() {
    if (running_) return;

    if (audio_.getDeviceCount() == 0) {
        throw std::runtime_error("No audio devices available.");
    }

    RtAudio::StreamParameters outParams;
    outParams.deviceId = audio_.getDefaultOutputDevice();
    outParams.nChannels = 1;  // mono output for now
    outParams.firstChannel = 0;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_MINIMIZE_LATENCY;

    try {
        audio_.openStream(
            &outParams,
            nullptr,
            RTAUDIO_SINT16,              // 16-bit signed
            sampleRate_,
            &bufferFrames_,
            &AudioRtBackend::audioCallback,
            this,
            &options
        );

        audio_.startStream();
        running_ = true;
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("RtAudio error: ") + e.what());
    } catch (...) {
        throw std::runtime_error("Unknown RtAudio error.");
    }
}

void AudioRtBackend::stop() {
    if (!running_) return;

    try {
        if (audio_.isStreamRunning()) {
            audio_.stopStream();
        }
        if (audio_.isStreamOpen()) {
            audio_.closeStream();
        }
    } catch (std::exception& e) {
        std::cerr << "RtAudio stop/close error: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Unknown RtAudio stop/close error.\n";
    }

    running_ = false;
}

int AudioRtBackend::audioCallback(void* outputBuffer,
                                  void* /*inputBuffer*/,
                                  unsigned int nFrames,
                                  double /*streamTime*/,
                                  RtAudioStreamStatus status,
                                  void* userData)
{
    auto* self = static_cast<AudioRtBackend*>(userData);
    auto* out  = static_cast<int16_t*>(outputBuffer);

    if (status) {
        // Optional: log XRuns / underflows
    }

    self->engine_.render(out, static_cast<uint16_t>(nFrames));
    return 0; // continue
}
