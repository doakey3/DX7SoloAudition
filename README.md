# DX7SoloAudition

DX7SoloAudition is a minimal, CLI, standalone **DX7 voice player** designed for fast,
real‑time auditioning of **single‑voice .syx patches** using a MIDI controller.

You can convert a DX7 cartridge to individual voices using [DXConvert](https://dxconvert.martintarenskeen.nl/).

Unlike Dexed or other full synthesizer environments, DX7SoloAudition focuses on
loading a single DX7 patch → play it using a MIDI controller

This makes it ideal for:
- Rapid patch browsing
- Curating DX7 instrument libraries
- Testing extracted voices from cartridges
- Using hardware MIDI keyboards to audition FM sounds
- Integrating into DX7Tracker workflows

---

## Features

- Loads **155‑byte or 163 byte DX7 voice data** (raw `.syx` single‑voice files)
- Real‑time audio output using **RtAudio**
- Real‑time MIDI input using **RtMidi**
- Velocity‑sensitive playback (if patch supports it)
- Auto‑detection of connected MIDI controllers
- Command‑line options for voice and port selection
- Lightweight, headless, instant startup

---

## Usage

```bash
./DX7SoloAudition --voice my_patch.syx
```

### Flags

```
--voice <file>        Load a single‑voice DX7 .syx file
--midi-port <index>   Select a specific MIDI input port
--help                Show command help
```

If no voice is provided, an **init patch** is used.

---

## MIDI Port Selection

Running:

```bash
./DX7SoloAudition --help
```

Displays available ports:

```
Available MIDI input ports:
  [0] Midi Through
  [1] Arturia KeyStep 37
```

To use port 1:

```bash
./DX7SoloAudition --midi-port 1 --voice EPiano.syx
```

---

## Build Instructions

Download the repository:

```bash
git clone --recursive https://github.com/doakey3/DX7SoloAudition
cd DX7SoloAudition
```

Build

```bash
mkdir build
cd build
cmake ..
make -j
```

Executable location:

```
./DX7SoloAudition
```

---

## License

Released into the public domain via **The Unlicense**.

You may use, modify, redistribute, embed, or sell this software freely,
without attribution.

See `UNLICENSE` for full text.

---

## Why this project?

Most DX7 tools are:
- Full synthesizers  
- Cartridge editors  
- VST plugins  
- Bulk librarians  

What *doesn't* exist is a fast, lightweight, one‑voice, real‑time audition tool.

**DX7SoloAudition** fills that gap, acting like a compact virtual DX7 rack module
for immediate patch testing.
