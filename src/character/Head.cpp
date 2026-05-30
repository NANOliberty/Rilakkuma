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

// 머즐(흰 주둥이) 배치 파라미터 — 한곳에서 관리
const float kMuzzleCY = -0.26f;  // 머리 로컬 y (얼굴 아래쪽)
const float kMuzzleCZ = 0.66f;   // 머리 로컬 z (앞쪽)
const float kMuzzleRX = 0.63f;   // 가로 반경 (넓은 타원)
const float kMuzzleRY = 0.40f;   // 세로 반경
const float kMuzzleRZ = 0.40f;   // 깊이 반경 (앞으로 살짝만 돌출)

//------------------------------------------------------------------------------
// 머리 메인 구 (단색 갈색)
//------------------------------------------------------------------------------
void renderHeadSphere() {
    Lighting::applyPlushMaterial();
    Palette::brown();
    // 실제 리락쿠마처럼 세로보다 가로가 살짝 넓은 머리(완전한 구보다 귀여움).
    // 자식 좌표계는 그대로 두고 메쉬만 스케일(앞쪽 z 는 유지).
    glPushMatrix();
    glScalef(1.05f, 0.97f, 1.0f);
    glutSolidSphere(kHeadRadius, 48, 48);
    glPopMatrix();
}

//------------------------------------------------------------------------------
// 눈: 검은 작은 구 + 아주 작은 흰 하이라이트
//------------------------------------------------------------------------------
void renderEye() {
    Lighting::applyPlushMaterial();
    Palette::eye();
    // 실제 리락쿠마처럼 크고 또렷한 눈(살짝 세로로 긴 타원).
    glPushMatrix();
    glScalef(0.92f, 1.05f, 0.85f);
    glutSolidSphere(0.155f, 28, 28);
    glPopMatrix();

    // 흰 하이라이트
    glPushMatrix();
    glTranslatef(0.05f, 0.06f, 0.11f);
    glColor3f(0.95f, 0.95f, 0.95f);
    glutSolidSphere(0.045f, 14, 14);
    glPopMatrix();
}

//------------------------------------------------------------------------------
// 귀: 납작한 갈색 구 + 앞면에 작고 납작한 노란 안쪽 (덜 튀어나오게)
//------------------------------------------------------------------------------
void renderEar() {
    Lighting::applyPlushMaterial();

    // 귀 본체 (갈색, 납작)
    Palette::brown();
    glPushMatrix();
    glScalef(1.0f, 1.0f, 0.55f);
    glutSolidSphere(0.40f, 32, 32);
    glPopMatrix();

    // 안쪽 노란색: 작고 납작한 오발을 앞면에 살짝만 얹음
    Palette::yellow();
    glPushMatrix();
    glTranslatef(0.0f, -0.01f, 0.18f);
    glScalef(0.55f, 0.70f, 0.22f);
    glutSolidSphere(0.30f, 24, 24);
    glPopMatrix();
}

//------------------------------------------------------------------------------
// 머즐(흰 주둥이) + 코+입 텍스처 데칼.
//  1) 흰 타원체를 그려 둥근 주둥이를 만든다(대부분 머리에 묻힘 -> 앞만 돌출).
//  2) 그 앞면에 코+입 텍스처를 곡면 데칼로 얹는다. 흰 배경이 머즐과 같은 색이라
//     사각 경계 없이 어두운 코+입만 자연스럽게 보인다.
//------------------------------------------------------------------------------
void renderMuzzle() {
    // --- 1) 흰 머즐 본체 ---
    Lighting::applyPlushMaterial();
    Palette::cream();
    glPushMatrix();
    glTranslatef(0.0f, kMuzzleCY, kMuzzleCZ);
    MeshUtils::renderEllipsoid(kMuzzleRX, kMuzzleRY, kMuzzleRZ, 40, 40);
    glPopMatrix();

    // --- 2) 코+입 텍스처 데칼 (머즐 앞면 곡면을 따라) ---
    const float halfU = 50.0f * kPi / 180.0f; // 좌우 반각
    const float halfV = 46.0f * kPi / 180.0f; // 상하 반각
    const int   N = 22;

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

    glPushMatrix();
    glTranslatef(0.0f, kMuzzleCY, kMuzzleCZ);
    for (int i = 0; i < N; ++i) {
        float v0 = (float)i / N;
        float v1 = (float)(i + 1) / N;
        float th0 = (v0 - 0.5f) * 2.0f * halfV;
        float th1 = (v1 - 0.5f) * 2.0f * halfV;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= N; ++j) {
            float u = (float)j / N;
            float ph = (u - 0.5f) * 2.0f * halfU;

            // 위 행 (v1)
            {
                float dx = cosf(th1) * sinf(ph);
                float dy = sinf(th1);
                float dz = cosf(th1) * cosf(ph);
                // 타원체 위 점 + 바깥으로 살짝(z-fighting 방지)
                float px = (kMuzzleRX + 0.012f) * dx;
                float py = (kMuzzleRY + 0.012f) * dy;
                float pz = (kMuzzleRZ + 0.012f) * dz;
                float nx = dx / kMuzzleRX, ny = dy / kMuzzleRY, nz = dz / kMuzzleRZ;
                float nl = sqrtf(nx*nx + ny*ny + nz*nz); if (nl < 1e-6f) nl = 1.0f;
                if (textured) glTexCoord2f(u, v1);
                glNormal3f(nx/nl, ny/nl, nz/nl);
                glVertex3f(px, py, pz);
            }
            // 아래 행 (v0)
            {
                float dx = cosf(th0) * sinf(ph);
                float dy = sinf(th0);
                float dz = cosf(th0) * cosf(ph);
                float px = (kMuzzleRX + 0.012f) * dx;
                float py = (kMuzzleRY + 0.012f) * dy;
                float pz = (kMuzzleRZ + 0.012f) * dz;
                float nx = dx / kMuzzleRX, ny = dy / kMuzzleRY, nz = dz / kMuzzleRZ;
                float nl = sqrtf(nx*nx + ny*ny + nz*nz); if (nl < 1e-6f) nl = 1.0f;
                if (textured) glTexCoord2f(u, v0);
                glNormal3f(nx/nl, ny/nl, nz/nl);
                glVertex3f(px, py, pz);
            }
        }
        glEnd();
    }
    glPopMatrix();

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

    // cream 머즐(코+입 텍스처)
    SceneNode* muzzle = new SceneNode();
    muzzle->setRenderFunction(renderMuzzle);
    head->addChild(muzzle);

    // 눈 (머즐 위쪽 양옆, 살짝 낮고 좁게 -> 실제 리락쿠마 느낌)
    SceneNode* leftEye = new SceneNode();
    leftEye->setTranslation(-0.40f, 0.20f, 0.84f);
    leftEye->setRenderFunction(renderEye);
    head->addChild(leftEye);

    SceneNode* rightEye = new SceneNode();
    rightEye->setTranslation(0.40f, 0.20f, 0.84f);
    rightEye->setRenderFunction(renderEye);
    head->addChild(rightEye);

    // 귀 (위쪽 양옆, 살짝 바깥으로 기울임)
    SceneNode* leftEar = new SceneNode();
    leftEar->setTranslation(-0.7f, 0.62f, 0.0f);
    leftEar->setRotation(20.0f, 0.0f, 0.0f, 1.0f);
    leftEar->setRenderFunction(renderEar);
    head->addChild(leftEar);

    SceneNode* rightEar = new SceneNode();
    rightEar->setTranslation(0.7f, 0.62f, 0.0f);
    rightEar->setRotation(-20.0f, 0.0f, 0.0f, 1.0f);
    rightEar->setRenderFunction(renderEar);
    head->addChild(rightEar);

    return head;
}
