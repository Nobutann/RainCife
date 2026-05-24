#!/bin/bash
# Extract history video frames at 30fps as PNG files.
# Usage: run from MSYS2 MinGW64 shell from project root.

INPUT="assets/sprites/result1779649951.mp4"
OUTPUT_DIR="assets/sprites/history_frames"
FPS=30

mkdir -p "$OUTPUT_DIR"

echo "Extracting frames from $INPUT at ${FPS}fps..."
/mingw64/bin/ffmpeg -y -i "$INPUT" -vf "fps=${FPS}" -f image2 "${OUTPUT_DIR}/frame_%04d.png"

COUNT=$(ls "$OUTPUT_DIR"/frame_*.png 2>/dev/null | wc -l)
echo "Done! Extracted $COUNT frames to $OUTPUT_DIR/"
