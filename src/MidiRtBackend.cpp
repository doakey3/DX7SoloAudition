#include "MidiRtBackend.h"
#include "DX7Engine.h"

#include <iostream>
#include <stdexcept>

MidiRtBackend::MidiRtBackend(DX7Engine& engine, int preferredPort)
    : midiIn_(std::make_unique<RtMidiIn>()),
      engine_(engine)
{
    unsigned int portCount = midiIn_->getPortCount();
    if (portCount == 0) {
        std::cerr << "No MIDI input ports available.\n";
        return;
    }

    std::cout << "Available MIDI input ports:\n";
    for (unsigned int i = 0; i < portCount; ++i) {
        std::cout << "  [" << i << "] " << midiIn_->getPortName(i) << "\n";
    }

    unsigned int portToOpen = 0;

    if (preferredPort >= 0 && preferredPort < (int)portCount) {
        portToOpen = preferredPort;
    } else {
        // Auto-select: pick the first device that is NOT “Midi Through”
        for (unsigned int i = 0; i < portCount; ++i) {
            std::string name = midiIn_->getPortName(i);
            if (name.find("Midi Through") == std::string::npos) {
                portToOpen = i;
                break;
            }
        }
    }

    try {
        midiIn_->openPort(portToOpen);
        midiIn_->setClientName("DX7Synth");
        midiIn_->setPortName("DX7Synth Input");
        std::cout << "Opened MIDI input port " << portToOpen
                  << ": " << midiIn_->getPortName(portToOpen) << "\n";
    }
    catch (RtMidiError& e) {
        throw std::runtime_error("RtMidi openPort error: " + e.getMessage());
    }

    midiIn_->ignoreTypes(false, true, true);
    midiIn_->setCallback(&MidiRtBackend::midiCallback, this);
}

MidiRtBackend::~MidiRtBackend() {
    if (midiIn_ && midiIn_->isPortOpen()) {
        midiIn_->closePort();
    }
}

void MidiRtBackend::midiCallback(double /*timeStamp*/,
                                 std::vector<unsigned char>* message,
                                 void* userData)
{
    if (!message || message->empty()) return;

    auto* self = static_cast<MidiRtBackend*>(userData);
    DX7Engine& engine = self->engine_;

    const auto& msg = *message;
    uint8_t status = msg[0];

    // Note On
    if ((status & 0xF0) == 0x90 && msg.size() >= 3) {
        uint8_t note = msg[1];
        uint8_t vel  = msg[2];
        if (vel == 0) {
            engine.noteOff(note);
        } else {
            engine.noteOn(note, vel);
        }
    }
    // Note Off
    else if ((status & 0xF0) == 0x80 && msg.size() >= 3) {
        uint8_t note = msg[1];
        engine.noteOff(note);
    }
}
