#pragma once

#include <RtMidi.h>
#include <cstdint>
#include <memory>
#include <vector>

class DX7Engine;

class MidiRtBackend {
public:
    explicit MidiRtBackend(DX7Engine& engine, int preferredPort = -1);
    ~MidiRtBackend();

private:
    std::unique_ptr<RtMidiIn> midiIn_;
    DX7Engine& engine_;

    static void midiCallback(double timeStamp,
                             std::vector<unsigned char>* message,
                             void* userData);
};
