cmake_minimum_required(VERSION 3.15)

project(DX7SoloAudition
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ============================
# Dependencies
# ============================

# Threads are cross-platform
find_package(Threads REQUIRED)

# ALSA only exists on Linux/UNIX (not macOS)
if(UNIX AND NOT APPLE)
    find_package(ALSA REQUIRED)
endif()

# ============================
# Synth_Dexed
# ============================

FetchContent_Declare(
    Synth_Dexed
    GIT_REPOSITORY  https://codeberg.org/dcoredump/Synth_Dexed.git
    GIT_TAG         master
)

FetchContent_MakeAvailable(Synth_Dexed)

# Note: FetchContent uses a lowercase name for *_SOURCE_DIR
file(GLOB SYNTH_DEXED_SOURCES
    "${synth_dexed_SOURCE_DIR}/src/*.cpp"
)

add_library(SynthDexed STATIC
    ${SYNTH_DEXED_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/compat/arm_math.cpp"
)

target_include_directories(SynthDexed PUBLIC
    "${synth_dexed_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/compat"
)

# ============================
# RtAudio
# ============================

FetchContent_Declare(
    RtAudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(RtAudio)

add_library(RtAudioLib STATIC
    "${rtaudio_SOURCE_DIR}/RtAudio.cpp"
)

target_include_directories(RtAudioLib PUBLIC
    "${rtaudio_SOURCE_DIR}"
)

# Choose RtAudio backend by platform
if(APPLE)
    # macOS: CoreAudio
    target_compile_definitions(RtAudioLib PRIVATE __MACOSX_CORE__)
elseif(WIN32)
    # Windows: WASAPI (you could switch to DS or ASIO if you want)
    target_compile_definitions(RtAudioLib PRIVATE __WINDOWS_WASAPI__)
else()
    # Linux / other UNIX: ALSA
    target_compile_definitions(RtAudioLib PRIVATE __LINUX_ALSA__)
    target_link_libraries(RtAudioLib PRIVATE ALSA::ALSA)
endif()

# Threads everywhere
target_link_libraries(RtAudioLib PRIVATE Threads::Threads)

# ============================
# RtMidi
# ============================

FetchContent_Declare(
    RtMidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(RtMidi)

add_library(RtMidiLib STATIC
    "${rtmidi_SOURCE_DIR}/RtMidi.cpp"
)

target_include_directories(RtMidiLib PUBLIC
    "${rtmidi_SOURCE_DIR}"
)

# Choose RtMidi backend by platform
if(APPLE)
    # macOS CoreMIDI
    target_compile_definitions(RtMidiLib PRIVATE __MACOSX_CORE__)
elseif(WIN32)
    # Windows multimedia MIDI
    target_compile_definitions(RtMidiLib PRIVATE __WINDOWS_MM__)
else()
    # Linux ALSA sequencer
    target_compile_definitions(RtMidiLib PRIVATE __LINUX_ALSA__)
    target_link_libraries(RtMidiLib PRIVATE ALSA::ALSA)
endif()

# ============================
# DX7SoloAudition executable
# ============================

file(GLOB DX7SoloAudition_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(DX7SoloAudition
    ${DX7SoloAudition_SOURCES}
)

target_include_directories(DX7SoloAudition
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Core libs (platform agnostic)
target_link_libraries(DX7SoloAudition
    PRIVATE
        SynthDexed
        RtAudioLib
        RtMidiLib
        Threads::Threads
)

# On Linux, also link ALSA explicitly in the app (harmless but safe)
if(UNIX AND NOT APPLE)
    target_link_libraries(DX7SoloAudition PRIVATE ALSA::ALSA)
endif()
