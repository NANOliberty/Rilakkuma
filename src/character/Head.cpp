//==============================================================================
// Head.cpp  -  리락쿠마 얼굴부 (A 담당)
//
//  좌표(PROJECT_SPEC 5.2.1)는 월드 기준. 머리 노드를 (0,1.5,0) 으로 옮기므로
//  머리의 자식들은 머리 로컬좌표(= 월드 - (0,1.5,0))로 배치한다.
//
//  v2 수정(스크린샷 피드백 반영):
//   - cream 영역을 "사각 패치" -> "둥근 흰 머즐(타원체)"로 변경. 대부분 머리에
//     묻히고 앞면만 살짝 둥글게 돌출 -> 옆에서 봐도 판때기/사각 경계가 안 보임.
//   - 코+입 텍스처는 머즐 앞면에 곡면 데칼로 매핑. 텍스처의 흰 배경이 머즐
//     흰색과 같아 데칼 사각 경계가 보이지 않는다(어두운 코+입만 보임).
//   - 귀 안쪽 노란색을 작고 납작하게 눕혀 옆면 돌출을 줄임.
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

// 얼굴 텍스처 패치 파라미터.
//  머리 표면에 거의 밀착(돌출 X)하는 곡면 패치에 face_cream.png 를 입힌다.
//  텍스처 배경이 머리 갈색과 같아 패치 경계는 보이지 않고, 가운데 흰 타원 +
//  코 + 입만 평평하게 나타난다(실제 리락쿠마처럼).
const float kFacePitch = -8.0f;  // 패치 중심을 아래로(도)
const float kFaceHalfU = 52.0f;  // 좌우 반각(도)
const float kFaceHalfV = 50.0f;  // 상하 반각(도)

//------------------------------------------------------------------------------
// 머리 메인 구 (단색 갈색)
//------------------------------------------------------------------------------
void renderHeadSphere() {
    Lighting::applyPlushMaterial();
    Palette::brown();
    // 순수 구로 그린다. (구를 스케일하면 표면 법선이 얼굴 텍스처 패치의 법선과
    // 어긋나 패치 사각 경계가 음영 차이로 드러난다. 구로 두면 패치가 머리와
    // 완전히 같은 색·조명이 되어 경계가 사라진다.)
    glutSolidSphere(kHeadRadius, 48, 48);
}

//------------------------------------------------------------------------------
// 눈: 진한 갈색의 납작한 점(실제 리락쿠마는 광택 없는 자수 느낌).
//------------------------------------------------------------------------------
void renderEye() {
    Lighting::applyPlushMaterial();
    Palette::eye();
    // 완전한 구로 그린다. 납작한 원반은 머리 옆면 곡률에 가려 한쪽이 잘려
    // 세로 타원처럼 보였다. 구는 어느 각도에서도 동그란 점으로 보인다.
    glutSolidSphere(0.105f, 28, 28);
}

//------------------------------------------------------------------------------
// 귀: 납작한 갈색 원반 + 안쪽-아래로 치우친 큰 노란 부분(원본처럼).
//  innerSign: 노란색을 귀의 "안쪽(머리 중심 쪽)"으로 밀어주는 부호.
//             왼쪽 귀(+24° 회전)는 +1, 오른쪽 귀(-24°)는 -1.
//------------------------------------------------------------------------------
void renderEar(float innerSign) {
    Lighting::applyPlushMaterial();

    // 귀 본체 (갈색, 납작하고 크게)
    Palette::brown();
    glPushMatrix();
    glScalef(1.0f, 1.0f, 0.5f);
    glutSolidSphere(0.46f, 32, 32);
    glPopMatrix();

    // 안쪽 노란색: 크게 + 안쪽-아래로 치우치게(원본의 초승달 느낌).
    // 바깥-위쪽에 갈색 테가 남아 자연히 초승달처럼 보인다.
    Palette::yellow();
    glPushMatrix();
    glTranslatef(innerSign * 0.13f, -0.11f, 0.20f);
    glScalef(0.74f, 0.86f, 0.30f);
    glutSolidSphere(0.42f, 28, 28);
    glPopMatrix();
}

//------------------------------------------------------------------------------
// 얼굴 텍스처 패치: 머리 표면에 거의 밀착하는 곡면에 face_cream.png 를 입힌다.
//  - 텍스처 배경 = 머리 갈색 -> 패치 사각 경계는 머리색과 섞여 사라짐.
//  - 가운데 흰 타원 + 코 + 입만 평평하게 보여 실제 리락쿠마처럼 됨(돌출 X).
//  - 패치 점의 법선은 머리 표면 법선과 같아 조명이 머리와 일치 -> 경계 무봉제.
//------------------------------------------------------------------------------
void renderFace() {
    const float R     = kHeadRadius + 0.01f;          // 표면에서 아주 살짝만
    const float halfU = kFaceHalfU * kPi / 180.0f;
    const float halfV = kFaceHalfV * kPi / 180.0f;
    const float p0    = kFacePitch * kPi / 180.0f;
    const int   N     = 30;

    bool textured = (g_faceTexId != 0);
    if (textured) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_faceTexId);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        Palette::cream();
    }
    Lighting::applyPlushMaterial();

    for (int i = 0; i < N; ++i) {
        float v0 = (float)i / N;
        float v1 = (float)(i + 1) / N;
        float th0 = p0 + (v0 - 0.5f) * 2.0f * halfV;
        float th1 = p0 + (v1 - 0.5f) * 2.0f * halfV;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= N; ++j) {
            float u  = (float)j / N;
            float ph = (u - 0.5f) * 2.0f * halfU;

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

    // 얼굴(흰 타원+코+입) 텍스처 패치 — 머리 표면에 납작하게
    SceneNode* face = new SceneNode();
    face->setRenderFunction(renderFace);
    head->addChild(face);

    // 눈 (완전한 구. 머리 밖으로 살짝 나와 어디서 봐도 동그랗게)
    SceneNode* leftEye = new SceneNode();
    leftEye->setTranslation(-0.40f, 0.0f, 0.94f);
    leftEye->setRenderFunction(renderEye);
    head->addChild(leftEye);

    SceneNode* rightEye = new SceneNode();
    rightEye->setTranslation(0.40f, 0.0f, 0.94f);
    rightEye->setRenderFunction(renderEye);
    head->addChild(rightEye);

    // 귀 (위쪽 양옆, 크게 + 바깥으로 기울임). 노란색은 각자 안쪽으로 치우치게.
    SceneNode* leftEar = new SceneNode();
    leftEar->setTranslation(-0.72f, 0.66f, 0.0f);
    leftEar->setRotation(24.0f, 0.0f, 0.0f, 1.0f);
    leftEar->setRenderFunction([]() { renderEar(+1.0f); });
    head->addChild(leftEar);

    SceneNode* rightEar = new SceneNode();
    rightEar->setTranslation(0.72f, 0.66f, 0.0f);
    rightEar->setRotation(-24.0f, 0.0f, 0.0f, 1.0f);
    rightEar->setRenderFunction([]() { renderEar(-1.0f); });
    head->addChild(rightEar);

    return head;
}
