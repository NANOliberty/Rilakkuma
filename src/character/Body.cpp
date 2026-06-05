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
        MeshUtils::renderEllipsoid(1.15f, 1.18f, 0.92f, 48, 40);
    }

    // --- 배 (흰 타원, 몸통 앞면에 납작하게) ---
    void renderBelly() {
        Lighting::applyPlushMaterial();
        Palette::belly();
        MeshUtils::renderEllipsoid(0.70f, 0.80f, 0.40f, 36, 32);
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
        renderPlushLimb(0.32f, 0.32f, 1.15f);

        // 손바닥(노란 패드) — 팔 끝 앞면에 크고 둥근 납작 타원.
        //  X축으로 눕혀 손바닥 면이 앞(+z)을 향하게 → 정면에서 패드가 잘 보임.
        glPushMatrix();
        glTranslatef(0.0f, -1.09f, 0.14f);
        glRotatef(78.0f, 1.0f, 0.0f, 0.0f);
        Palette::yellow();
        MeshUtils::renderEllipsoid(0.22f, 0.24f, 0.09f, 28, 20);
        glPopMatrix();
    }

    // --- 다리 (둥근 발 + 노란 발바닥). 이제 가운데 '한 다리'로만 쓰므로 통통하게. ---
    void renderLeg() {
        Lighting::applyPlushMaterial();
        Palette::brown();

        // 다리(허벅지~정강이): 통통한 타원체.
        renderPlushLimb(0.44f, 0.48f, 1.05f);

        // 발: 앞으로 살짝 튀어나온 둥근 발등(갈색)
        glPushMatrix();
        glTranslatef(0.0f, -1.04f, 0.14f);
        MeshUtils::renderEllipsoid(0.44f, 0.34f, 0.56f, 28, 24);
        glPopMatrix();

        // 노란 발바닥: 발 앞-아래 면에 크고 둥근 납작 패드. X축으로 눕혀
        //  패드 면이 앞-아래를 향하게 → 발을 앞으로 들면 정면에서 살짝 보인다.
        glPushMatrix();
        glTranslatef(0.0f, -1.22f, 0.34f);
        glRotatef(70.0f, 1.0f, 0.0f, 0.0f);
        Palette::yellow();
        MeshUtils::renderEllipsoid(0.30f, 0.34f, 0.11f, 28, 20);
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
            float zPos = -0.90f + zCurveOffset;

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

        float sliderZ = -0.90f + ((sliderY * sliderY) * 0.30f) - 0.04f;
        glTranslatef(0.0f, sliderY, sliderZ);

        glRotatef(30.0f, 1.0f, 0.0f, 0.0f);

        glutSolidCube(0.07f);

        glTranslatef(0.0f, -0.07f, 0.0f);
        glutSolidTorus(0.015f, 0.05f, 10, 16);
        glPopMatrix();
    }

} // namespace


SceneNode* BuildBody() {

    // 몸 루트. 커진 몸통(ry=1.18) 윗면(y = -0.50 + 1.18 = 0.68)이 머리 아랫면
    // (y≈0.34)을 0.34 파고들어 목 없이 머리-몸이 이어진다.
    SceneNode* root = new SceneNode();
    root->setTranslation(0.0f, -0.50f, 0.0f);

    // 상체(몸통). 똑바로 세워 머리·몸·팔이 한 축으로 정렬.
    SceneNode* torso = new SceneNode();
    torso->setRenderFunction(renderBody);
    root->addChild(torso);

    // --- 흰 배 (몸통 앞면에 부착. 커진 몸에 맞춰 더 크고 앞으로) ---
    SceneNode* belly = new SceneNode();
    belly->setTranslation(0.0f, -0.15f, 0.66f);
    belly->setRenderFunction(renderBelly);
    torso->addChild(belly);

    // --- 왼팔: 번쩍 든 인사(웨이브) 포즈 ---
    //  어깨 기준 Z축 -155° → 거의 수직으로 위로, 끝이 머리 옆으로. X -12°는 살짝 앞.
    SceneNode* leftShoulder = new SceneNode();
    leftShoulder->setTranslation(-0.95f, 0.55f, 0.08f);
    leftShoulder->setRotation(-155.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* leftArmX = new SceneNode();
    leftArmX->setRotation(-12.0f, 1.0f, 0.0f, 0.0f);
    leftArmX->setRenderFunction(renderArm);
    leftShoulder->addChild(leftArmX);
    torso->addChild(leftShoulder);

    // --- 오른팔: 배 쪽으로 내려 살짝 안고 있는 자세 ---
    //  Z -30°(아래+안쪽) + X -38°(앞으로 당겨 배 위에 손이 오게).
    //  ※ X 부호 주의: -Y 로 내려온 팔을 +X축 기준 음각으로 돌려야 손이 앞(+z)으로
    //    온다. 양각이면 뒤(-z)로 돌아가 몸통 뒤에 숨어 '팔이 사라진' 것처럼 보인다.
    SceneNode* rightShoulder = new SceneNode();
    rightShoulder->setTranslation(0.95f, 0.55f, 0.08f);
    rightShoulder->setRotation(-30.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* rightArmX = new SceneNode();
    rightArmX->setRotation(-38.0f, 1.0f, 0.0f, 0.0f);
    rightArmX->setRenderFunction(renderArm);
    rightShoulder->addChild(rightArmX);
    torso->addChild(rightShoulder);

    // --- 꼬리 (뒤쪽 작은 갈색 구. 커진 몸 등면에 맞춰 뒤로) ---
    SceneNode* tail = new SceneNode();
    tail->setTranslation(0.0f, -0.50f, -0.92f);
    tail->setRenderFunction([]() {
        Lighting::applyPlushMaterial();
        Palette::brown();
        glutSolidSphere(0.24f, 24, 24);
        });
    torso->addChild(tail);

    // --- 등 지퍼 ---
    SceneNode* zipper = new SceneNode();
    zipper->setRenderFunction(renderZipper);
    torso->addChild(zipper);

    // --- 다리: '한 다리'만. 몸 아래-가운데에서 앞으로 약간 떠 있게 두고, 발을
    //     앞으로 들어(X축 -28°) 노란 발바닥이 정면에서 살짝 보이게 한다. ---
    SceneNode* legPivot = new SceneNode();
    legPivot->setTranslation(0.0f, -0.70f, 0.28f);   // 바닥 중앙, 살짝 앞으로 띄움
    legPivot->setRotation(-28.0f, 1.0f, 0.0f, 0.0f); // 발끝을 앞-위로 → 발바닥 노출
    legPivot->setRenderFunction(renderLeg);
    root->addChild(legPivot);

    return root;
}