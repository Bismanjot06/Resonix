# Resonix

JUCE-based standalone music app scaffold with:

- drum pads
- 16-step sequencer
- mixer controls
- oscillator synth
- pattern save/load
- WAV export

## Build

Configure with CMake and make sure the build machine can fetch JUCE from GitHub, or replace the FetchContent block in `CMakeLists.txt` with your local JUCE path.

## Assets

Place real drum samples in `Source/Assets/` as `kick.wav`, `snare.wav`, and `hats.wav`. The app falls back to synthesized clicks if the files are missing or empty.
