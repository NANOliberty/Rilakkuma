#!/usr/bin/env python3
"""
make_face_texture.py  -  리락쿠마 얼굴 cream 영역용 텍스처(face_cream.png) 생성기.

외부 라이브러리(PIL 등) 없이 표준 라이브러리(zlib, struct)만으로 512x512 PNG 를
직접 인코딩한다. 흰색(warm) 배경 + 중앙에 검은 코 + Y/ω 형태의 입.

재생성:  python3 tools/make_face_texture.py
출력:    assets/textures/face_cream.png
"""
import zlib, struct, math, os

W = H = 512

# 색상
BG   = (245, 238, 220)   # #F5EEDC warm white (cream)
DARK = (51, 38, 31)      # 코/입 자수 색 (0.20,0.15,0.12)

# 프레임버퍼 (RGB)
buf = bytearray()
for _ in range(W * H):
    buf += bytes(BG)

def put(x, y, c):
    if 0 <= x < W and 0 <= y < H:
        i = (y * W + x) * 3
        buf[i:i+3] = bytes(c)

def fill_ellipse(cx, cy, rx, ry, c):
    for y in range(int(cy - ry), int(cy + ry) + 1):
        for x in range(int(cx - rx), int(cx + rx) + 1):
            dx = (x - cx) / rx
            dy = (y - cy) / ry
            if dx * dx + dy * dy <= 1.0:
                put(x, y, c)

def stamp(cx, cy, r, c):
    """반지름 r 의 채워진 원을 찍어 두꺼운 선/곡선을 만든다."""
    for y in range(int(cy - r), int(cy + r) + 1):
        for x in range(int(cx - r), int(cx + r) + 1):
            if (x - cx) ** 2 + (y - cy) ** 2 <= r * r:
                put(x, y, c)

def stroke_path(points, r, c):
    for (x, y) in points:
        stamp(x, y, r, c)

# --- 코: 둥근 삼각형 느낌의 가로 타원 ---
fill_ellipse(256, 212, 46, 33, DARK)

# --- 코 아래 -> 입 갈림점 수직선 ---
vline = [(256, y) for y in range(243, 286)]
stroke_path(vline, 7, DARK)

# --- 입: 두 개의 아래로 볼록한 호(∪∪ = 리락쿠마 입) ---
def lower_arc(cx, cy, r, steps=160):
    pts = []
    for k in range(steps + 1):
        a = math.pi * k / steps          # 0..pi -> 아래쪽 반원
        pts.append((cx + r * math.cos(a), cy + r * math.sin(a)))
    return pts

stroke_path(lower_arc(214, 284, 43), 7, DARK)   # 왼쪽 입
stroke_path(lower_arc(298, 284, 43), 7, DARK)   # 오른쪽 입

# ---------- PNG 인코딩 ----------
def png_chunk(tag, data):
    chunk = tag + data
    return (struct.pack(">I", len(data)) + chunk +
            struct.pack(">I", zlib.crc32(chunk) & 0xffffffff))

# 각 스캔라인 앞에 filter byte(0) 추가
raw = bytearray()
for y in range(H):
    raw.append(0)
    raw += buf[y * W * 3:(y + 1) * W * 3]

sig = b"\x89PNG\r\n\x1a\n"
ihdr = struct.pack(">IIBBBBB", W, H, 8, 2, 0, 0, 0)  # 8-bit, color type 2 (RGB)
idat = zlib.compress(bytes(raw), 9)

png = sig + png_chunk(b"IHDR", ihdr) + png_chunk(b"IDAT", idat) + png_chunk(b"IEND", b"")

out_dir = os.path.join(os.path.dirname(__file__), "..", "assets", "textures")
out_dir = os.path.normpath(out_dir)
os.makedirs(out_dir, exist_ok=True)
out_path = os.path.join(out_dir, "face_cream.png")
with open(out_path, "wb") as f:
    f.write(png)
print("wrote", out_path, len(png), "bytes")
