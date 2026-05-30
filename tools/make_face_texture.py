#!/usr/bin/env python3
"""
make_face_texture.py  -  리락쿠마 얼굴 cream 영역용 텍스처(face_cream.png) 생성기.

외부 라이브러리(PIL 등) 없이 표준 라이브러리(zlib, struct)만으로 512x512 PNG 를
직접 인코딩한다. 배경 = 머리 갈색(텍스처를 얼굴에 납작하게 입혔을 때 경계가
머리색과 섞여 사라짐) + 중앙에 작은 흰 cream 타원 + 코 + ω 형태의 입.

가장자리 계단현상(울퉁불퉁)을 없애기 위해 SS 배 고해상도로 그린 뒤 평균으로
축소한다(슈퍼샘플링 안티앨리어싱).

재생성:  python3 tools/make_face_texture.py
출력:    assets/textures/face_cream.png
"""
import zlib, struct, math, os

W = H = 512
SS = 4                 # 슈퍼샘플 배수 (4x4 평균 -> 부드러운 가장자리)
BW = W * SS            # 작업(고해상도) 폭

# 색상
BG    = (194, 128, 61)   # head brown (Palette::brown 0.761,0.502,0.239 과 동일)
CREAM = (245, 238, 220)  # 흰 cream 타원
DARK  = (51, 38, 31)     # 코/입

# 고해상도 프레임버퍼
buf = bytearray(bytes(BG) * (BW * BW))

def put(x, y, c):
    if 0 <= x < BW and 0 <= y < BW:
        i = (y * BW + x) * 3
        buf[i:i+3] = bytes(c)

# 아래 그리기 함수들은 "논리 좌표(512 기준)"를 받아 내부에서 SS 배로 확대한다.
def fill_ellipse(cx, cy, rx, ry, c):
    cx, cy, rx, ry = cx*SS, cy*SS, rx*SS, ry*SS
    for y in range(int(cy - ry), int(cy + ry) + 1):
        for x in range(int(cx - rx), int(cx + rx) + 1):
            dx = (x - cx) / rx
            dy = (y - cy) / ry
            if dx * dx + dy * dy <= 1.0:
                put(x, y, c)

def stamp(cx, cy, r, c):
    cx, cy, r = cx*SS, cy*SS, r*SS
    rr = r * r
    for y in range(int(cy - r), int(cy + r) + 1):
        for x in range(int(cx - r), int(cx + r) + 1):
            if (x - cx) ** 2 + (y - cy) ** 2 <= rr:
                put(x, y, c)

def stroke_path(points, r, c):
    for (x, y) in points:
        stamp(x, y, r, c)

def fill_tri(p0, p1, p2, c):
    pts = [(p[0]*SS, p[1]*SS) for p in (p0, p1, p2)]
    ys = [p[1] for p in pts]
    y0, y1 = int(min(ys)), int(max(ys))
    def edges_x(y):
        xs = []
        for i in range(3):
            ax, ay = pts[i]
            bx, by = pts[(i + 1) % 3]
            if (ay <= y < by) or (by <= y < ay):
                t = (y - ay) / (by - ay)
                xs.append(ax + t * (bx - ax))
        return xs
    for y in range(y0, y1 + 1):
        xs = edges_x(y)
        if len(xs) >= 2:
            for x in range(int(min(xs)), int(max(xs)) + 1):
                put(x, y, c)

def qbez(p0, p1, p2, steps=200):
    """2차 베지에 곡선 점들."""
    pts = []
    for k in range(steps + 1):
        t = k / steps
        x = (1-t)**2*p0[0] + 2*(1-t)*t*p1[0] + t*t*p2[0]
        y = (1-t)**2*p0[1] + 2*(1-t)*t*p1[1] + t*t*p2[1]
        pts.append((x, y))
    return pts

# ---------------- 얼굴 그리기 (논리 512 좌표) ----------------
CX = 256

# 흰 cream 타원 (작게)
fill_ellipse(CX, 288, 84, 60, CREAM)

# 코: 작은 둥근 역삼각형(아래로 살짝 뾰족, 끝이 둥근)
fill_ellipse(CX, 262, 30, 15, DARK)
fill_tri((CX - 30, 262), (CX + 30, 262), (CX, 283), DARK)
stamp(CX, 281, 8, DARK)

# 입: 코 끝 바로 아래에서 좌우로 갈라지는 짧은 두 획(人). 컴팩트하게.
SPLIT = (CX, 283)
stroke_path(qbez(SPLIT, (CX - 19, 295), (CX - 31, 301)), 5, DARK)
stroke_path(qbez(SPLIT, (CX + 19, 295), (CX + 31, 301)), 5, DARK)

# ---------------- SS 배 -> 512 다운샘플(박스 평균) ----------------
out = bytearray(W * H * 3)
for y in range(H):
    for x in range(W):
        r = g = b = 0
        base_y = y * SS
        base_x = x * SS
        for yy in range(SS):
            row = ((base_y + yy) * BW + base_x) * 3
            for xx in range(SS):
                p = row + xx * 3
                r += buf[p]; g += buf[p+1]; b += buf[p+2]
        n = SS * SS
        o = (y * W + x) * 3
        out[o] = r // n; out[o+1] = g // n; out[o+2] = b // n

# ---------------- PNG 인코딩 ----------------
def png_chunk(tag, data):
    chunk = tag + data
    return (struct.pack(">I", len(data)) + chunk +
            struct.pack(">I", zlib.crc32(chunk) & 0xffffffff))

raw = bytearray()
for y in range(H):
    raw.append(0)
    raw += out[y * W * 3:(y + 1) * W * 3]

png = (b"\x89PNG\r\n\x1a\n"
       + png_chunk(b"IHDR", struct.pack(">IIBBBBB", W, H, 8, 2, 0, 0, 0))
       + png_chunk(b"IDAT", zlib.compress(bytes(raw), 9))
       + png_chunk(b"IEND", b""))

out_dir = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", "assets", "textures"))
os.makedirs(out_dir, exist_ok=True)
out_path = os.path.join(out_dir, "face_cream.png")
with open(out_path, "wb") as f:
    f.write(png)
print("wrote", out_path, len(png), "bytes  (supersample x%d)" % SS)
