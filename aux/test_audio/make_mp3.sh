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

SAMPLE_RATE="44100"       # Standard sample rate (44.1 kHz)
BIT_RATE="128k"           # Constant Bit Rate (CBR)
CHANNELS="1"              # Mono audio (simplest channel mode)
ENCODER="libmp3lame"      # Standard MP3 encoder

# no idv3 metadata
ID3_TAGS="-map_metadata -1 -write_id3v1 0 -id3v2_version 0"
# constant bitrate
CBR_MODE="-b:a $BIT_RATE -global_quality 0"

SILENT_WAV="silence.wav"

# silent wav
ffmpeg -f lavfi -i anullsrc=channel_layout=mono:sample_rate="$SAMPLE_RATE" -t 1 -c:a pcm_s16le "$OUTDIR/$SILENT_WAV"

# encode mp3
ffmpeg -i "$INPUT" $ID3_TAGS -acodec "$ENCODER" -ac "$CHANNELS" -ar "$SAMPLE_RATE" $CBR_MODE "$OUTDIR/$(base).mp3"
ffmpeg -i "$OUTDIR/$SILENT_WAV" $ID3_TAGS -acodec "$ENCODER" -ac "$CHANNELS" -ar "$SAMPLE_RATE" $CBR_MODE "$OUTDIR/silence.mp3"

# remove tmp wav
rm -rf "$OUTDIR/$SILENT_WAV"
