//==============================================================================
// Head.cpp  -  리락쿠마 얼굴부 (A 담당)
//
//  좌표(PROJECT_SPEC 5.2.1)는 모두 "월드 기준"으로 주어져 있다. 머리 노드를
//  (0,1.5,0) 으로 옮기므로, 머리의 자식들은 (월드좌표 - (0,1.5,0)) = 머리
//  로컬좌표로 배치한다.
//
//    cream 영역 중심 (0,1.3,0.85) -> 로컬 (0,-0.2,0.85)
//    왼쪽 눈        (-0.45,1.7,0.85) -> 로컬 (-0.45,0.2,0.85)
//    오른쪽 눈      (+0.45,1.7,0.85) -> 로컬 (+0.45,0.2,0.85)
//    왼쪽 귀 중심   (-0.7,2.1,0)     -> 로컬 (-0.7,0.6,0)
//    오른쪽 귀 중심 (+0.7,2.1,0)     -> 로컬 (+0.7,0.6,0)
//==============================================================================
#include "Rilakkuma.h"
#include "../core/SceneNode.h"
#include "../core/MeshUtils.h"
#include "../core/Lighting.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>
#include <cmath>

// Rilakkuma.cpp 에서 정의. 얼굴 cream 텍스처 ID (0 이면 단색 폴백).
extern GLuint g_faceTexId;

namespace {
const float kPi = 3.14159265358979323846f;
const float kHeadRadius = 1.0f;

//------------------------------------------------------------------------------
// 머리 메인 구 (단색 갈색)
//------------------------------------------------------------------------------
void renderHeadSphere() {
    Lighting::applyPlushMaterial();
    Palette::brown();
    glutSolidSphere(kHeadRadius, 48, 48);
}

//------------------------------------------------------------------------------
// 눈: 검은 작은 구 + 아주 작은 흰 하이라이트
//------------------------------------------------------------------------------
void renderEye() {
    Lighting::applyPlushMaterial();
    Palette::eye();
    glutSolidSphere(0.13f, 24, 24);

    // 하이라이트 (살짝 +z, +y 로 띄움)
    glPushMatrix();
    glTranslatef(0.04f, 0.05f, 0.10f);
    glColor3f(0.9f, 0.9f, 0.9f);
    glutSolidSphere(0.035f, 12, 12);
    glPopMatrix();
}

//------------------------------------------------------------------------------
// 귀: 납작한 갈색 구 + 앞면(+z)에 작은 노란 원 포인트
//------------------------------------------------------------------------------
void renderEar() {
    Lighting::applyPlushMaterial();

    // 귀 본체 (갈색, 살짝 납작하게)
    Palette::brown();
    glPushMatrix();
    glScalef(1.0f, 1.0f, 0.6f);
    glutSolidSphere(0.42f, 32, 32);
    glPopMatrix();

    // 안쪽 노란 원 포인트 (앞쪽으로 살짝 돌출)
    Palette::yellow();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.30f);
    glScalef(1.0f, 1.0f, 0.4f);
    glutSolidSphere(0.22f, 24, 24);
    glPopMatrix();
}

//------------------------------------------------------------------------------
// cream 영역 = 얼굴 앞쪽에 붙는 곡면 텍스처 패치(코+입 자수).
//
//  머리 반지름 R 구의 앞면(+z) 일부를 살짝 띄워(R+eps) 패치로 그린다.
//  patch 중심은 아래로 centerPitch 만큼 내려 입 주변에 오게 한다.
//  (u,v) in [0,1]^2 -> 패치 전체에 텍스처가 한 번 매핑되므로, 텍스처 중앙의
//  코+입 자수가 얼굴 정면 중앙에 자연스럽게 표시된다.
//------------------------------------------------------------------------------
void renderCreamPatch() {
    const float R = kHeadRadius + 0.015f; // 표면보다 살짝 바깥(z-fighting 방지)
    const float halfU = 42.0f * kPi / 180.0f; // 좌우 반각
    const float halfV = 34.0f * kPi / 180.0f; // 상하 반각
    const float pitch0 = -12.0f * kPi / 180.0f; // 패치 중심을 아래로
    const int   N = 24; // 분할 수

    bool textured = (g_faceTexId != 0);
    if (textured) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_faceTexId);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1.0f, 1.0f, 1.0f); // 텍스처 색 그대로(조명만 반영)
    } else {
        Palette::cream(); // 폴백: cream 단색
    }
    Lighting::applyPlushMaterial();

    for (int i = 0; i < N; ++i) {
        float v0 = (float)i / N;
        float v1 = (float)(i + 1) / N;
        // v -> pitch (위가 v=1)
        float th0 = pitch0 + (v0 - 0.5f) * 2.0f * halfV;
        float th1 = pitch0 + (v1 - 0.5f) * 2.0f * halfV;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= N; ++j) {
            float u = (float)j / N;
            float ph = (u - 0.5f) * 2.0f * halfU; // yaw

            // 위 행 (v1)
            {
                float dx = cosf(th1) * sinf(ph);
                float dy = sinf(th1);
                float dz = cosf(th1) * cosf(ph);
                if (textured) glTexCoord2f(u, v1);
                glNormal3f(dx, dy, dz);
                glVertex3f(R * dx, R * dy, R * dz);
            }
            // 아래 행 (v0)
            {
                float dx = cosf(th0) * sinf(ph);
                float dy = sinf(th0);
                float dz = cosf(th0) * cosf(ph);
                if (textured) glTexCoord2f(u, v0);
                glNormal3f(dx, dy, dz);
                glVertex3f(R * dx, R * dy, R * dz);
            }
        }
        glEnd();
    }

    if (textured) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

} // namespace

//------------------------------------------------------------------------------
// 머리 서브트리 조립
//------------------------------------------------------------------------------
SceneNode* BuildHead() {
    SceneNode* head = new SceneNode();
    head->setTranslation(0.0f, 1.5f, 0.0f);   // 머리 중심 (0,1.5,0)
    head->setRenderFunction(renderHeadSphere);

    // cream 영역(코+입 텍스처) — 머리 앞면에 곡면 패치
    SceneNode* cream = new SceneNode();
    cream->setRenderFunction(renderCreamPatch);
    head->addChild(cream);

    // 왼쪽 눈 (로컬 -0.45, 0.2, 0.85)
    SceneNode* leftEye = new SceneNode();
    leftEye->setTranslation(-0.45f, 0.2f, 0.85f);
    leftEye->setRenderFunction(renderEye);
    head->addChild(leftEye);

    // 오른쪽 눈 (로컬 +0.45, 0.2, 0.85)
    SceneNode* rightEye = new SceneNode();
    rightEye->setTranslation(0.45f, 0.2f, 0.85f);
    rightEye->setRenderFunction(renderEye);
    head->addChild(rightEye);

    // 왼쪽 귀 (로컬 -0.7, 0.6, 0), 살짝 바깥으로 기울임
    SceneNode* leftEar = new SceneNode();
    leftEar->setTranslation(-0.7f, 0.6f, 0.0f);
    leftEar->setRotation(20.0f, 0.0f, 0.0f, 1.0f);
    leftEar->setRenderFunction(renderEar);
    head->addChild(leftEar);

    // 오른쪽 귀 (로컬 +0.7, 0.6, 0)
    SceneNode* rightEar = new SceneNode();
    rightEar->setTranslation(0.7f, 0.6f, 0.0f);
    rightEar->setRotation(-20.0f, 0.0f, 0.0f, 1.0f);
    rightEar->setRenderFunction(renderEar);
    head->addChild(rightEar);

    return head;
}
