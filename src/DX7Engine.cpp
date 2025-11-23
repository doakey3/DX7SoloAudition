#include "DX7Engine.h"

#include <fstream>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <cmath>

DX7Engine::DX7Engine(double sampleRate, uint8_t maxNotes)
    : sampleRate_(sampleRate),
      dexed_(maxNotes, static_cast<uint32_t>(sampleRate_))
{
    dexed_.activate();
    dexed_.loadInitVoice();
    dexed_.setGain(0.5f); // tweak to taste
}

bool DX7Engine::extractVoice155FromSysex(const uint8_t* data,
                                         std::size_t len,
                                         uint8_t outVoice[155])
{
    if (!data || len < 6 + 155 + 2) {
        return false;
    }

    // 1. Find F0
    std::size_t start = 0;
    while (start < len && data[start] != 0xF0) {
        ++start;
    }
    if (start == len) {
        return false;
    }

    // 2. Find last F7
    std::size_t end = len;
    while (end > start && data[end - 1] != 0xF7) {
        --end;
    }
    if (end <= start) {
        return false;
    }

    std::size_t frameLen = end - start; // F0..F7 inclusive

    // 3. Must be at least header(6) + voice(155) + checksum(1) + F7(1)
    if (frameLen < 6 + 155 + 2) {
        std::cerr << "Sysex frame too short: " << frameLen << " bytes\n";
        return false;
    }

    // 4. Extract the 155-byte voice parameter block
    const uint8_t* voice = data + start + 6;
    std::memcpy(outVoice, voice, 155);
    return true;
}

bool DX7Engine::loadVoiceFromMemory(const uint8_t* data, std::size_t len) {
    if (!data || len == 0) return false;

    if (len == voiceData_.size()) {
        // Already a raw 155-byte voice block
        std::memcpy(voiceData_.data(), data, voiceData_.size());
        dexed_.loadVoiceParameters(voiceData_.data());
        return true;
    }

    // Otherwise try to parse as SysEx frame (DX7 single-voice style)
    if (!extractVoice155FromSysex(data, len, voiceData_.data())) {
        return false;
    }

    dexed_.loadVoiceParameters(voiceData_.data());
    return true;
}

bool DX7Engine::loadVoiceFromFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        std::cerr << "Failed to open voice file: " << path << "\n";
        return false;
    }

    std::vector<uint8_t> bytes(
        (std::istreambuf_iterator<char>(f)),
        std::istreambuf_iterator<char>());

    return loadVoiceFromMemory(bytes.data(), bytes.size());
}

void DX7Engine::noteOn(uint8_t note, uint8_t velocity) {
    uint8_t v = mapVelocity(velocity);
    dexed_.keydown(note, v);
}

void DX7Engine::noteOff(uint8_t note) {
    dexed_.keyup(note);
}

void DX7Engine::render(int16_t* buffer, uint16_t nFrames) {
    if (!buffer || nFrames == 0) return;
    dexed_.render(buffer, nFrames);
}

void DX7Engine::setVelocityCurve(VelocityCurve curve) {
    velCurve_ = curve;
}

uint8_t DX7Engine::mapVelocity(uint8_t raw) const {
    if (raw == 0) return 0;

    float v = static_cast<float>(raw) / 127.0f;
    float mapped = v;

    switch (velCurve_) {
    case VelocityCurve::LinearFull:
        mapped = v;
        break;
    case VelocityCurve::Soft:
        mapped = std::sqrt(v);
        break;
    case VelocityCurve::Hard:
        mapped = v * v;
        break;
    }

    int out = static_cast<int>(mapped * 127.0f + 0.5f);
    if (out < 1)   out = 1;
    if (out > 127) out = 127;
    return static_cast<uint8_t>(out);
}
