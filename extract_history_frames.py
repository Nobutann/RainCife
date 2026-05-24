"""
extract_history_frames.py
-----------------------
Extracts frames from assets/sprites/result1779649951.mp4 into assets/sprites/history_frames/
at 30 fps using OpenCV or pyav or ffmpeg subprocess.

Run from the project root:  python extract_history_frames.py
"""

import os
import sys
import subprocess

INPUT  = os.path.join("assets", "sprites", "result1779649951.mp4")
OUTDIR = os.path.join("assets", "sprites", "history_frames")
FPS    = 30

os.makedirs(OUTDIR, exist_ok=True)

# ── Try OpenCV first ──────────────────────────────────────────────────────────
try:
    import cv2
    cap = cv2.VideoCapture(INPUT)
    if not cap.isOpened():
        raise RuntimeError(f"Cannot open {INPUT}")

    src_fps  = cap.get(cv2.CAP_PROP_FPS) or FPS
    interval = max(1, round(src_fps / FPS))
    idx      = 0
    saved    = 0

    while True:
        ret, frame = cap.read()
        if not ret:
            break
        if idx % interval == 0:
            saved += 1
            path = os.path.join(OUTDIR, f"frame_{saved:04d}.png")
            cv2.imwrite(path, frame)
        idx += 1

    cap.release()
    print(f"[cv2] Extracted {saved} frames to {OUTDIR}/")
    sys.exit(0)

except ImportError:
    pass
except Exception as e:
    print(f"[cv2] Error: {e}")

# ── Try pyav ──────────────────────────────────────────────────────────────────
try:
    import av
    from PIL import Image

    container = av.open(INPUT)
    stream    = container.streams.video[0]
    src_fps   = float(stream.average_rate) if stream.average_rate else FPS
    interval  = max(1, round(src_fps / FPS))
    idx       = 0
    saved     = 0

    for packet in container.demux(stream):
        for frame in packet.decode():
            if idx % interval == 0:
                saved += 1
                img  = frame.to_image()
                path = os.path.join(OUTDIR, f"frame_{saved:04d}.png")
                img.save(path)
            idx += 1

    print(f"[pyav] Extracted {saved} frames to {OUTDIR}/")
    sys.exit(0)

except ImportError:
    pass
except Exception as e:
    print(f"[pyav] Error: {e}")

# ── Fallback: call ffmpeg subprocess ─────────────────────────────────────────
ffmpeg_candidates = [
    r"C:\msys64\ucrt64\bin\ffmpeg.exe",
    r"C:\msys64\mingw64\bin\ffmpeg.exe",
    "ffmpeg",
]

ffmpeg_exe = None
for candidate in ffmpeg_candidates:
    if os.path.isfile(candidate) or candidate == "ffmpeg":
        ffmpeg_exe = candidate
        break

if ffmpeg_exe is None:
    print("ERROR: No ffmpeg found. Install opencv-python or pyav, or put ffmpeg in PATH.")
    sys.exit(1)

out_pattern = os.path.join(OUTDIR, "frame_%04d.png")

# Build environment with UCRT64 DLLs first to avoid conflicts
env = os.environ.copy()
env["PATH"] = r"C:\msys64\ucrt64\bin;" + env.get("PATH", "")

cmd = [ffmpeg_exe, "-y", "-i", INPUT, "-vf", f"fps={FPS}", "-f", "image2", out_pattern]
print(f"[ffmpeg] Running: {' '.join(cmd)}")

result = subprocess.run(cmd, env=env, capture_output=True, text=True)
if result.returncode != 0:
    print(f"[ffmpeg] STDERR:\n{result.stderr}")
    # Try again with mingw64 path
    env["PATH"] = r"C:\msys64\mingw64\bin;" + env.get("PATH", "")
    result = subprocess.run(cmd, env=env, capture_output=True, text=True)

if result.returncode == 0:
    count = len([f for f in os.listdir(OUTDIR) if f.startswith("frame_") and f.endswith(".png")])
    print(f"[ffmpeg] Extracted {count} frames to {OUTDIR}/")
else:
    print(f"[ffmpeg] Failed:\n{result.stderr}")
    sys.exit(1)
