# AV Speech in Noise

AV Speech in Noise provides a macOS interface for presenting audio-visual stimuli at precise levels, collecting participant responses and saving trial data to a local file.

# Build
```
$ cmake -S . -B build -GXcode -DCMAKE_Swift_LANGUAGE_VERSION=5
$ cmake --build build --target av-speech-in-noise-macos-bundle --config Release
```
