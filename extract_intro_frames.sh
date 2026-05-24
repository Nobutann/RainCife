#!/bin/bash
# Extract intro video frames at 30fps as PNG files for the raylib intro screen.
# Usage: run from MSYS2 MinGW64 shell from project root.

INPUT="assets/sprites/Intro.mp4"
OUTPUT_DIR="assets/sprites/intro_frames"
FPS=30

mkdir -p "$OUTPUT_DIR"

echo "Extracting frames from $INPUT at ${FPS}fps..."
/mingw64/bin/ffmpeg -y -i "$INPUT" -vf "fps=${FPS}" -f image2 "${OUTPUT_DIR}/frame_%04d.png"

COUNT=$(ls "$OUTPUT_DIR"/frame_*.png 2>/dev/null | wc -l)
echo "Done! Extracted $COUNT frames to $OUTPUT_DIR/"
