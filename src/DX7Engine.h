#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <string>

#include "dexed.h"  // from external/Synth_Dexed/src

// Thin wrapper to expose the protected getSamples() as a public method.
class DexedPlayer : public Dexed {
public:
    DexedPlayer(uint8_t maxnotes, uint32_t rate)
        : Dexed(maxnotes, rate) {}

    void render(int16_t* buffer, uint16_t nSamples) {
        // getSamples is protected, but accessible to subclasses
        getSamples(buffer, nSamples);
    }
};

// Simple host-side velocity curves.
// Most of the real velocity behavior is in the patch itself.
enum class VelocityCurve {
    LinearFull, // raw 0–127
    Soft,       // more dynamic at low velocities
    Hard        // more emphasis on high velocities
};

class DX7Engine {
public:
    DX7Engine(double sampleRate, uint8_t maxNotes = 16);
    ~DX7Engine() = default;

    // Accepts either:
    //  - a 163-byte DX7 single-voice sysex file (DXConvert output), or
    //  - a raw 155-byte voice data file.
    bool loadVoiceFromFile(const std::string& path);

    // Load from memory: either 155 bytes or full SysEx frame (we'll parse).
    bool loadVoiceFromMemory(const uint8_t* data, std::size_t len);

    // Velocity curve (host-side)
    void setVelocityCurve(VelocityCurve curve);
    VelocityCurve velocityCurve() const { return velCurve_; }

    // MIDI-ish interface
    void noteOn(uint8_t note, uint8_t velocity);
    void noteOff(uint8_t note);

    // Render mono samples (16-bit). nFrames == number of samples.
    void render(int16_t* buffer, uint16_t nFrames);

    double sampleRate() const { return sampleRate_; }

private:
    double      sampleRate_;
    DexedPlayer dexed_;  // engine instance

    // 155-byte voice parameter block (what Synth_Dexed expects).
    std::array<uint8_t, 155> voiceData_;

    // Host-side velocity curve.
    VelocityCurve velCurve_ = VelocityCurve::LinearFull;
    uint8_t       mapVelocity(uint8_t raw) const;

    // Extract a 155-byte DX7 voice from a SysEx buffer.
    // Mirrors your Python strip_syx() logic.
    static bool extractVoice155FromSysex(const uint8_t* data,
                                         std::size_t len,
                                         uint8_t outVoice[155]);

    DX7Engine(const DX7Engine&) = delete;
    DX7Engine& operator=(const DX7Engine&) = delete;
};
