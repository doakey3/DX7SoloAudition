#include "DX7Engine.h"
#include "AudioRtBackend.h"
#include "MidiRtBackend.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

void printHelp() {
    std::cout <<
"DX7SoloAudition – real-time DX7 voice engine\n\n"
"Usage:\n"
"  DX7SoloAudition [options]\n\n"
"Options:\n"
"  --voice <file.syx>        Load a specific DX7 voice file\n"
"  --midi-port <index>       Open a specific MIDI input port\n"
"  --velocity-curve <name>   Set velocity curve: linear, soft, hard\n"
"  --help                    Show this help message\n\n";
}

int main(int argc, char** argv) {
    std::string syxPath;
    int midiPortOverride = -1;
    std::string velCurveName = "linear";

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help")) {
            printHelp();
            return 0;
        }
        else if (!strcmp(argv[i], "--voice") && i + 1 < argc) {
            syxPath = argv[++i];
        }
        else if (!strcmp(argv[i], "--midi-port") && i + 1 < argc) {
            midiPortOverride = std::stoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "--velocity-curve") && i + 1 < argc) {
            velCurveName = argv[++i];
        }
        else {
            std::cerr << "Unknown option: " << argv[i] << "\n";
            printHelp();
            return 1;
        }
    }

    const double sampleRate = 48000.0;
    const unsigned int bufferFrames = 256;

    try {
        DX7Engine engine(sampleRate, 16);

        // Choose velocity curve
        VelocityCurve curve = VelocityCurve::LinearFull;
        if (velCurveName == "linear")   curve = VelocityCurve::LinearFull;
        else if (velCurveName == "soft")   curve = VelocityCurve::Soft;
        else if (velCurveName == "hard")   curve = VelocityCurve::Hard;
        else {
            std::cerr << "Unknown velocity curve '" << velCurveName
                      << "'. Using linear.\n";
        }
        engine.setVelocityCurve(curve);

        if (!syxPath.empty()) {
            if (!engine.loadVoiceFromFile(syxPath)) {
                std::cerr << "Failed to load .syx file: " << syxPath << "\n";
            }
        } else {
            std::cout << "No .syx file specified; using init voice.\n";
        }

        AudioRtBackend audio(engine, sampleRate, bufferFrames);
        audio.start();

        MidiRtBackend midi(engine, midiPortOverride);

        std::cout << "DX7SoloAudition running at " << sampleRate << " Hz.\n"
                  << "Velocity curve: " << velCurveName << "\n"
                  << "Ctrl+C to quit.\n";

        for (;;) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}
