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
- [Multimedia Programming Interface and Data Specifications 1.0](https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/Docs/riffmci.pdf)
- [ISO/IEC 11172-3:1993 - MPEG-1 Audio (Layer I, II, III) (Committee Draft)](https://csclub.uwaterloo.ca/~pbarfuss/ISO11172-3.pdf)
