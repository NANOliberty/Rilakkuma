#include "MeshUtils.h"
#include <cmath>

namespace {
const float kPi = 3.14159265358979323846f;
} // namespace

namespace MeshUtils {

//------------------------------------------------------------------------------
// 반구: 구면 좌표 (theta: 위도 0~pi/2, phi: 경도 0~2pi) 로 정점을 생성.
// 평평한 단면은 y=0, 볼록한 쪽이 +y. 법선은 구이므로 위치 벡터와 동일.
//------------------------------------------------------------------------------
void renderHemisphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; ++i) {
        float theta0 = (kPi * 0.5f) * (float)i / stacks;       // 아래 위도
        float theta1 = (kPi * 0.5f) * (float)(i + 1) / stacks; // 위 위도

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float phi = 2.0f * kPi * (float)j / slices;
            float cphi = cosf(phi), sphi = sinf(phi);

            // 위쪽 위도 링
            float nx1 = sinf(theta1) * cphi;
            float ny1 = cosf(theta1);
            float nz1 = sinf(theta1) * sphi;
            glNormal3f(nx1, ny1, nz1);
            glVertex3f(radius * nx1, radius * ny1, radius * nz1);

            // 아래쪽 위도 링
            float nx0 = sinf(theta0) * cphi;
            float ny0 = cosf(theta0);
            float nz0 = sinf(theta0) * sphi;
            glNormal3f(nx0, ny0, nz0);
            glVertex3f(radius * nx0, radius * ny0, radius * nz0);
        }
        glEnd();
    }
}

//------------------------------------------------------------------------------
// 위/아래 단면이 다른 타원 실린더. y 를 따라 장/단반경을 선형 보간.
// 옆면 법선은 타원면의 외향 근사(접선의 수직)로 계산한다.
//------------------------------------------------------------------------------
void renderTaperedEllipticCylinder(
    float bottomMajor, float bottomMinor,
    float topMajor, float topMinor,
    float height, int slices, int stacks) {

    float yBottom = -height * 0.5f;

    // ---- 옆면 ----
    for (int i = 0; i < stacks; ++i) {
        float t0 = (float)i / stacks;
        float t1 = (float)(i + 1) / stacks;

        float y0 = yBottom + height * t0;
        float y1 = yBottom + height * t1;

        float a0 = bottomMajor + (topMajor - bottomMajor) * t0; // x 반경
        float b0 = bottomMinor + (topMinor - bottomMinor) * t0; // z 반경
        float a1 = bottomMajor + (topMajor - bottomMajor) * t1;
        float b1 = bottomMinor + (topMinor - bottomMinor) * t1;

        // y 에 따른 반경 변화율(법선의 y 성분 보정용)
        float dadt = (topMajor - bottomMajor) / height;
        float dbdt = (topMinor - bottomMinor) / height;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float phi = 2.0f * kPi * (float)j / slices;
            float cphi = cosf(phi), sphi = sinf(phi);

            // 타원면 법선: (b*cos, (기울기 보정), a*sin) 정규화
            float nx = b1 * cphi;
            float nz = a1 * sphi;
            float ny = -(a1 * b1) * (dadt * cphi * cphi + dbdt * sphi * sphi) / 1.0f;
            float len = sqrtf(nx * nx + ny * ny + nz * nz);
            if (len < 1e-6f) len = 1.0f;
            glNormal3f(nx / len, ny / len, nz / len);
            glVertex3f(a1 * cphi, y1, b1 * sphi);

            float nx0 = b0 * cphi;
            float nz0 = a0 * sphi;
            float ny0 = -(a0 * b0) * (dadt * cphi * cphi + dbdt * sphi * sphi) / 1.0f;
            float len0 = sqrtf(nx0 * nx0 + ny0 * ny0 + nz0 * nz0);
            if (len0 < 1e-6f) len0 = 1.0f;
            glNormal3f(nx0 / len0, ny0 / len0, nz0 / len0);
            glVertex3f(a0 * cphi, y0, b0 * sphi);
        }
        glEnd();
    }

    // ---- 아래 뚜껑 ----
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, yBottom, 0.0f);
    for (int j = slices; j >= 0; --j) {
        float phi = 2.0f * kPi * (float)j / slices;
        glVertex3f(bottomMajor * cosf(phi), yBottom, bottomMinor * sinf(phi));
    }
    glEnd();

    // ---- 위 뚜껑 ----
    float yTop = height * 0.5f;
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, yTop, 0.0f);
    for (int j = 0; j <= slices; ++j) {
        float phi = 2.0f * kPi * (float)j / slices;
        glVertex3f(topMajor * cosf(phi), yTop, topMinor * sinf(phi));
    }
    glEnd();
}

//------------------------------------------------------------------------------
// 캡슐: 중앙 실린더 + 위/아래 반구. 축은 y.
//------------------------------------------------------------------------------
void renderCapsule(float radius, float height, int slices, int stacks) {
    float halfH = height * 0.5f;

    // 중앙 실린더 옆면
    glBegin(GL_QUAD_STRIP);
    for (int j = 0; j <= slices; ++j) {
        float phi = 2.0f * kPi * (float)j / slices;
        float cphi = cosf(phi), sphi = sinf(phi);
        glNormal3f(cphi, 0.0f, sphi);
        glVertex3f(radius * cphi, halfH, radius * sphi);
        glVertex3f(radius * cphi, -halfH, radius * sphi);
    }
    glEnd();

    // 위쪽 반구
    glPushMatrix();
    glTranslatef(0.0f, halfH, 0.0f);
    renderHemisphere(radius, slices, stacks);
    glPopMatrix();

    // 아래쪽 반구 (뒤집어서)
    glPushMatrix();
    glTranslatef(0.0f, -halfH, 0.0f);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    renderHemisphere(radius, slices, stacks);
    glPopMatrix();
}

//------------------------------------------------------------------------------
// 타원체 코어. wantTex 가 true 면 구면 UV 텍스처 좌표를 함께 낸다.
//------------------------------------------------------------------------------
static void ellipsoidCore(float rx, float ry, float rz,
                          int slices, int stacks, bool wantTex) {
    for (int i = 0; i < stacks; ++i) {
        float theta0 = kPi * (float)i / stacks;       // 0 .. pi (위->아래)
        float theta1 = kPi * (float)(i + 1) / stacks;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float phi = 2.0f * kPi * (float)j / slices;
            float cphi = cosf(phi), sphi = sinf(phi);

            // --- 위쪽 위도 링 ---
            {
                float sx = sinf(theta1) * cphi;
                float sy = cosf(theta1);
                float sz = sinf(theta1) * sphi;
                // 타원체 법선: (x/rx^2, y/ry^2, z/rz^2) 방향
                float nx = (sx) / (rx);
                float ny = (sy) / (ry);
                float nz = (sz) / (rz);
                float len = sqrtf(nx * nx + ny * ny + nz * nz);
                if (len < 1e-6f) len = 1.0f;
                if (wantTex) {
                    float u = (float)j / slices;
                    float v = 1.0f - (float)(i + 1) / stacks;
                    glTexCoord2f(u, v);
                }
                glNormal3f(nx / len, ny / len, nz / len);
                glVertex3f(rx * sx, ry * sy, rz * sz);
            }
            // --- 아래쪽 위도 링 ---
            {
                float sx = sinf(theta0) * cphi;
                float sy = cosf(theta0);
                float sz = sinf(theta0) * sphi;
                float nx = (sx) / (rx);
                float ny = (sy) / (ry);
                float nz = (sz) / (rz);
                float len = sqrtf(nx * nx + ny * ny + nz * nz);
                if (len < 1e-6f) len = 1.0f;
                if (wantTex) {
                    float u = (float)j / slices;
                    float v = 1.0f - (float)i / stacks;
                    glTexCoord2f(u, v);
                }
                glNormal3f(nx / len, ny / len, nz / len);
                glVertex3f(rx * sx, ry * sy, rz * sz);
            }
        }
        glEnd();
    }
}

void renderEllipsoid(float rx, float ry, float rz, int slices, int stacks) {
    ellipsoidCore(rx, ry, rz, slices, stacks, false);
}

void renderTexturedEllipsoid(float rx, float ry, float rz, GLuint texId,
                             int slices, int stacks) {
    if (texId != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texId);
        // 텍스처 색을 그대로 쓰되 조명은 받도록 MODULATE
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1.0f, 1.0f, 1.0f);
        ellipsoidCore(rx, ry, rz, slices, stacks, true);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    } else {
        // 폴백: 텍스처 로드 실패 시 cream 색 단색
        glColor3f(0.96f, 0.92f, 0.85f);
        ellipsoidCore(rx, ry, rz, slices, stacks, false);
    }
}

} // namespace MeshUtils
