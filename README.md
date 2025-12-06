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
    - Defines the **RIFF** container, including the **WAV** audio format
- [ISO/IEC 11172-3:1993 - MPEG-1 Audio (Layer I, II, III) (158 pages)](not telling you)
    - **Core MP3 spec**, frame structure, headers, and essential tables (Huffman, etc.)
- [ISO/IEC 13818-3:1997 - MPEG-2 Audio (127 pages)](bsolutly not telling you)
    - **MP3 extension**, Adds support for **LSF** for modern MP3 files.
- [ISO/IEC TR 11172-5:1998 - MPEG-1 Software Simulation (404 pages)](there are ways)
    - **Reference C code**
