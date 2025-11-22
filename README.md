# cPlayer

a wav player

### Supported formats
- [x] Raw PCM 24bit
- [ ] Float 32bit

later maybe
- [ ] more raw PCM formats, e.g. AIFF, AU
- [ ] ADCPM encoded WAV

the big maybes
- [ ] losslessly compressed data, e.g. FLAC
- [ ] ogg vorbis
- [ ] opus
- [ ] mp3 (probably won't)

### Dependencies
- `portaudio` is pulled by cmake but you may need to install its dependencies, e.g. `libasound2-dev`

### References
- [wav format specification](https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html)
