#include "Camera.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>
#include <cmath>

namespace {
const float kPi = 3.14159265358979323846f;
float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
} // namespace

Camera::Camera()
    : theta(0.0f),      // 정면
      phi(15.0f),       // 살짝 위에서 내려봄 (얼굴 디테일 보임)
      distance(8.0f),
      tx(0.0f), ty(0.5f), tz(0.0f), // 캐릭터 약간 위쪽
      minDist(2.5f), maxDist(25.0f) {}

void Camera::apply() const {
    float t = theta * kPi / 180.0f;
    float p = phi * kPi / 180.0f;

    // 구면 좌표 -> 카메라 위치
    float camX = tx + distance * cosf(p) * sinf(t);
    float camY = ty + distance * sinf(p);
    float camZ = tz + distance * cosf(p) * cosf(t);

    gluLookAt(camX, camY, camZ,
              tx, ty, tz,
              0.0f, 1.0f, 0.0f);
}

void Camera::addOrbit(float dTheta, float dPhi) {
    theta += dTheta;
    phi = clampf(phi + dPhi, -85.0f, 85.0f); // 짐벌 뒤집힘 방지
}

void Camera::zoom(float factor) {
    distance = clampf(distance * factor, minDist, maxDist);
}

void Camera::setTarget(float x, float y, float z) {
    tx = x; ty = y; tz = z;
}
