//==============================================================================
// Body.cpp  -  몸통부 (B 담당)
//==============================================================================
//
//  몸통 + 흰 배 / 양팔(인사·웨이브 포즈) / 양다리 / 등 지퍼 / 꼬리 구현.
//
//  ■ 형태 원칙(둥글둥글한 봉제인형 느낌)
//     - 팔다리는 '캡슐(직선 원기둥+반구)' 대신 직선 구간이 없는 '길쭉한 타원체'
//       (renderPlushLimb)로 그린다. 캡슐의 곧은 중앙부가 '알약/기둥'처럼 보이던
//       문제를 없앤다.
//     - 몸통은 넓고 둥근 타원체로, 머리(반지름 1.0)와 목폭이 맞게 폭을 키운다.
//     - 상체는 똑바로 세워(기울이지 않음) 머리·몸·팔이 한 축으로 정렬되게 한다.
//
//------------------------------------------------------------------------------
//  ■ 좌표 규약 (PROJECT_SPEC 5.2.1, 월드 기준 — 몸 루트는 원점이므로 그대로 사용)
//      몸통 중심        (0, 0, 0)         반지름 약 1.3~1.5
//      왼쪽 어깨(캐릭터) (-1.2, 0.5, 0)
//      오른쪽 어깨       (+1.2, 0.5, 0)
//      왼쪽 다리 시작점  (-0.6, -1.2, 0)
//      오른쪽 다리 시작점(+0.6, -1.2, 0)
//      꼬리             (0, 0, -1.4)
//
//  ■ 인사 포즈 회전 (PROJECT_SPEC 5.2.2)
//      왼팔(번쩍): Z축 +130°, X축 -15° 추가
//      오른팔(배쪽): Z축 -25°, X축 +25° 추가
//      왼다리: Z축 -10° / 오른다리: Z축 +10°
//
//  ■ 회전 기준점 주의
//      팔/다리는 "어깨/고관절"이 회전 중심이 되어야 자연스럽다. 즉 팔 메쉬는
//      로컬에서 위쪽 끝이 원점(0,0,0)에 오도록 그리고(아래로 길게 내려오는
//      형태), 그 노드를 어깨 위치로 translate + 회전한다.
//      예) 길이 L 인 팔이면 renderArm 안에서 y = 0 ~ -L 범위로 그린다.
//
//  ■ 두 축(Z + X) 회전을 합성하려면 노드를 중첩하세요:
//      SceneNode* shoulder = new SceneNode();        // 위치 + Z 회전
//      shoulder->setTranslation(-1.2f, 0.5f, 0.0f);
//      shoulder->setRotation(130.0f, 0,0,1);
//      SceneNode* armX = new SceneNode();            // X 회전
//      armX->setRotation(-15.0f, 1,0,0);
//      armX->setRenderFunction(renderArm);
//      shoulder->addChild(armX);
//      body->addChild(shoulder);
//
//  ■ 추천 메쉬 (MeshUtils 에 이미 구현되어 있음)
//      MeshUtils::renderTaperedEllipticCylinder(...)  // 팔다리 몸통
//      MeshUtils::renderCapsule(r, h)                 // 둥근 팔다리
//      MeshUtils::renderEllipsoid(rx,ry,rz)           // 흰 배, 손/발바닥 노란 타원
//      glutSolidSphere / glutSolidTorus               // 몸통, 꼬리, 지퍼 고리
//
//  ■ 색상은 Palette:: (brown/belly/yellow/zipper) 사용. 각 부위 렌더 직전
//      Lighting::applyPlushMaterial() 호출로 인형 질감 유지.
//
//  ■ 등 지퍼: (0,?,-1.3) 부근에서 목->꼬리 방향으로 작은 glutSolidTorus 를
//      일정 간격 반복 + 가는 실린더 레일. 자세한 건 docs/B_GUIDE.md 참고.
//==============================================================================
#include "Rilakkuma.h"
#include "../core/SceneNode.h"
#include "../core/MeshUtils.h"
#include "../core/Lighting.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>

namespace {

    // --- 리락쿠마 몸통 ---
    //  넓고 둥근 타원체. 머리(반지름 1.0)와 목폭이 맞도록 폭을 키우고(1.0),
    //  세로는 살짝만 길게(1.05) 해서 통통한 인형 비례를 만든다. (이전: 0.9×1.1×0.7
    //  → 좁고 길쭉해 머리와 어긋나 보였다.)
    void renderBody() {
        Lighting::applyPlushMaterial();
        Palette::brown();
        MeshUtils::renderEllipsoid(1.0f, 1.05f, 0.82f, 48, 40);
    }

    // --- 배 (흰 타원, 몸통 앞면에 납작하게) ---
    void renderBelly() {
        Lighting::applyPlushMaterial();
        Palette::belly();
        MeshUtils::renderEllipsoid(0.60f, 0.72f, 0.34f, 36, 32);
    }

    //--------------------------------------------------------------------------
    //  통통한 봉제 팔다리 핵심
    //
    //  '알약(캡슐)'처럼 보이던 이유 = 가운데 직선 원기둥 구간 때문. 직선 구간이
    //  전혀 없는 '길쭉한 타원체' 한 덩어리로 그리면 어느 각도에서도 둥글둥글한
    //  인형 팔다리가 된다. 로컬 원점(0,0,0)을 어깨/고관절에 두고 -y 로 내려온다.
    //
    //   bulkRX/RZ : 굵기(좌우/앞뒤 반경)   length : 팔다리 길이(원점→끝)
    //--------------------------------------------------------------------------
    void renderPlushLimb(float bulkRX, float bulkRZ, float length) {
        // 길쭉한 타원체. 중심을 length/2 만큼 내려 위 끝이 원점(어깨)에 닿게.
        glPushMatrix();
        glTranslatef(0.0f, -length * 0.5f, 0.0f);
        MeshUtils::renderEllipsoid(bulkRX, length * 0.5f, bulkRZ, 32, 28);
        glPopMatrix();
    }

    // --- 팔 (좌우 공용. 좌우 차이는 어깨 회전이 담당) ---
    void renderArm() {
        Lighting::applyPlushMaterial();
        Palette::brown();
        renderPlushLimb(0.30f, 0.30f, 1.05f);

        // 손바닥(노란 패드) — 팔 끝 앞면에 납작한 타원
        glPushMatrix();
        glTranslatef(0.0f, -0.98f, 0.07f);
        glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
        Palette::yellow();
        MeshUtils::renderEllipsoid(0.19f, 0.10f, 0.19f, 24, 16);
        glPopMatrix();
    }

    // --- 다리 (노란 발바닥 통합) ---
    void renderLeg() {
        Lighting::applyPlushMaterial();
        Palette::brown();
        renderPlushLimb(0.40f, 0.44f, 0.95f);

        // 발바닥 노란 타원 — 아래 앞쪽으로 살짝
        glPushMatrix();
        glTranslatef(0.0f, -0.86f, 0.12f);
        Palette::yellow();
        MeshUtils::renderEllipsoid(0.30f, 0.16f, 0.40f, 24, 16);
        glPopMatrix();
    }

    // --- 등 지퍼 렌더 함수 ---
    void renderZipper() {
        Lighting::applyPlushMaterial();
        Palette::zipper();

        for (int i = 0; i < 15; ++i) {
            glPushMatrix();
            float yPos = 0.55f - i * 0.075f;

            float zCurveOffset = (yPos * yPos) * 0.30f;
            float zPos = -0.80f + zCurveOffset;

            glTranslatef(0.0f, yPos, zPos);

            // 세로선 (지퍼 레일)
            glPushMatrix();
            float tiltAngle = yPos * 35.0f;
            glRotatef(tiltAngle, 1.0f, 0.0f, 0.0f);
            MeshUtils::renderCapsule(0.012f, 0.085f);
            glPopMatrix();

            // 가로선 (지퍼 이빨)
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidTorus(0.015f, 0.045f, 10, 16);
            glPopMatrix();
        }

        // 2. 지퍼 손잡이
        glPushMatrix();
        float sliderY = 0.62f;

        float sliderZ = -0.80f + ((sliderY * sliderY) * 0.30f) - 0.04f;
        glTranslatef(0.0f, sliderY, sliderZ);

        glRotatef(30.0f, 1.0f, 0.0f, 0.0f);

        glutSolidCube(0.07f);

        glTranslatef(0.0f, -0.07f, 0.0f);
        glutSolidTorus(0.015f, 0.05f, 10, 16);
        glPopMatrix();
    }

} // namespace


SceneNode* BuildBody() {

    // 몸 루트. 살짝 아래로 내려 머리(아래 끝 y=0.5)가 몸통 위(y≈0.45)에 자연스럽게
    // 묻히도록 한다 → 목 이음매가 안 보임.
    SceneNode* root = new SceneNode();
    root->setTranslation(0.0f, -0.60f, 0.0f);

    // 상체(몸통). 이전엔 -15° 기울여서 머리(안 기울어짐)와 따로 놀았다 → 똑바로
    // 세워 머리·몸·팔이 한 축으로 정렬되게 한다.
    SceneNode* torso = new SceneNode();
    torso->setRenderFunction(renderBody);
    root->addChild(torso);

    // --- 흰 배 (몸통 앞면에 부착) ---
    SceneNode* belly = new SceneNode();
    belly->setTranslation(0.0f, -0.12f, 0.60f);
    belly->setRenderFunction(renderBelly);
    torso->addChild(belly);

    // --- 왼팔: 번쩍 든 인사(웨이브) 포즈 ---
    //  어깨 기준 Z축 -155° → 거의 수직으로 위로, 끝이 머리 옆으로. X -12°는 살짝 앞.
    SceneNode* leftShoulder = new SceneNode();
    leftShoulder->setTranslation(-0.82f, 0.45f, 0.05f);
    leftShoulder->setRotation(-155.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* leftArmX = new SceneNode();
    leftArmX->setRotation(-12.0f, 1.0f, 0.0f, 0.0f);
    leftArmX->setRenderFunction(renderArm);
    leftShoulder->addChild(leftArmX);
    torso->addChild(leftShoulder);

    // --- 오른팔: 배 쪽으로 내려 살짝 안고 있는 자세 ---
    //  Z -30°(아래+안쪽) + X +38°(앞으로 당겨 배 위에 손이 오게).
    SceneNode* rightShoulder = new SceneNode();
    rightShoulder->setTranslation(0.82f, 0.45f, 0.05f);
    rightShoulder->setRotation(-30.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* rightArmX = new SceneNode();
    rightArmX->setRotation(38.0f, 1.0f, 0.0f, 0.0f);
    rightArmX->setRenderFunction(renderArm);
    rightShoulder->addChild(rightArmX);
    torso->addChild(rightShoulder);

    // --- 꼬리 (뒤쪽 작은 갈색 구) ---
    SceneNode* tail = new SceneNode();
    tail->setTranslation(0.0f, -0.45f, -0.82f);
    tail->setRenderFunction([]() {
        Lighting::applyPlushMaterial();
        Palette::brown();
        glutSolidSphere(0.22f, 24, 24);
        });
    torso->addChild(tail);

    // --- 등 지퍼 ---
    SceneNode* zipper = new SceneNode();
    zipper->setRenderFunction(renderZipper);
    torso->addChild(zipper);

    // --- 왼쪽 다리: 살짝 벌리고 앞으로 가볍게 (생기 있는 스탠스) ---
    SceneNode* leftLegPivot = new SceneNode();
    leftLegPivot->setTranslation(-0.48f, -0.62f, 0.10f);
    leftLegPivot->setRotation(-14.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* leftLegX = new SceneNode();
    leftLegX->setRotation(-12.0f, 1.0f, 0.0f, 0.0f);   // 앞으로 살짝 내딛음
    leftLegX->setRenderFunction(renderLeg);
    leftLegPivot->addChild(leftLegX);
    root->addChild(leftLegPivot);

    // --- 오른쪽 다리: 반대로 벌림 ---
    SceneNode* rightLegPivot = new SceneNode();
    rightLegPivot->setTranslation(0.48f, -0.62f, 0.10f);
    rightLegPivot->setRotation(14.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* rightLegMesh = new SceneNode();
    rightLegMesh->setRenderFunction(renderLeg);
    rightLegPivot->addChild(rightLegMesh);
    root->addChild(rightLegPivot);

    return root;
}