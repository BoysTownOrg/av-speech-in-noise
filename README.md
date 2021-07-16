# AV Speech in Noise
AV Speech in Noise provides a macOS interface for presenting audio-visual stimuli at precise levels, collecting participant responses and saving trial data to a local file.

# Build
```
$ cmake -S . -B build -GXcode -DCMAKE_Swift_LANGUAGE_VERSION=5
$ cmake --build build --target av-speech-in-noise-macos-bundle --config Release
```

# Resources
The application resource files are found [here](https://osf.io/r6ceh/). These files include images for the Consonant Test, a list of keywords for the Choose Keywords Test, a BTNRH logo, and preconfigured test settings used in the Facemask Study.
