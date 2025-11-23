include(FetchContent)

cmake_minimum_required(VERSION 3.15)

project(DX7SoloAudition
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================
# Dependencies
# ============================
find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

# ============================
# Synth_Dexed
# ============================

FetchContent_Declare(
    Synth_Dexed
    GIT_REPOSITORY  https://codeberg.org/dcoredump/Synth_Dexed.git
    GIT_TAG         master
)

FetchContent_MakeAvailable(Synth_Dexed)

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
    "${RtAudio_SOURCE_DIR}/RtAudio.cpp"
)

target_include_directories(RtAudioLib PUBLIC
    "${RtAudio_SOURCE_DIR}"
)

target_compile_definitions(RtAudioLib PRIVATE __LINUX_ALSA__)

target_link_libraries(RtAudioLib PRIVATE ALSA::ALSA Threads::Threads)



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
    "${RtMidi_SOURCE_DIR}/RtMidi.cpp"
)

target_include_directories(RtMidiLib PUBLIC
    "${RtMidi_SOURCE_DIR}"
)

target_compile_definitions(RtMidiLib PRIVATE __LINUX_ALSA__)
target_link_libraries(RtMidiLib PRIVATE ALSA::ALSA)


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

target_link_libraries(DX7SoloAudition
    PRIVATE
        SynthDexed
        RtAudioLib
        RtMidiLib
        ALSA::ALSA
        Threads::Threads
)
