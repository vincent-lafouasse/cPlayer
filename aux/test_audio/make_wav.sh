#!/bin/bash

set -o xtrace

if [[ $# -lt 2 ]]; then
    echo "Usage: $0 <input.wav> <output_dir>"
    exit 1
fi

INPUT="$1"
OUTDIR="$2"

mkdir -p "$OUTDIR"

base() {
    fname="$(basename "$INPUT")"
    echo "${fname%.*}"
}

SAMPLE_RATES="8000 16000 22050 44100 48000 96000"

# sample rate variations
for sampleRate in $SAMPLE_RATES; do
    ffmpeg -y -i "$INPUT" -ar "$sampleRate" "$OUTDIR/$(base)_${sampleRate}Hz.wav"
done

ENCODERS="
    pcm_u8
    pcm_s16le
    pcm_s24le
    pcm_s32le

    pcm_f32le
    pcm_f64le

    pcm_alaw
    pcm_mulaw
"

DEFAULT_SAMPLE_RATE=44100

# 2. bit depth
for codec in $ENCODERS; do
    ffmpeg -y -i "$INPUT" -f wav -ar "$DEFAULT_SAMPLE_RATE" -c:a "$codec" "$OUTDIR/$(base)_${codec}.wav"
done

# 4. Mono / Stereo variants
ffmpeg -y -i "$INPUT" -ac 1 "$OUTDIR/$(base)_mono.wav"
ffmpeg -y -i "$INPUT" -ac 2 "$OUTDIR/$(base)_stereo.wav"

# 5. Shortened clips (useful for boundary tests)
ffmpeg -y -i "$INPUT" -t 0.1 "$OUTDIR/$(base)_100ms.wav"
ffmpeg -y -i "$INPUT" -t 1.0 "$OUTDIR/$(base)_1s.wav"

# 6. Long padded version (silence appended)
ffmpeg -y -i "$INPUT" -af "apad=pad_dur=5" "$OUTDIR/$(base)_padded.wav"

echo "Done. Files written to: $OUTDIR"
