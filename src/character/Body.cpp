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

    //--------------------------------------------------------------------------
    //  둥근 기둥 (원기둥 느낌)
    //
    //  가운데에 '곧은 옆면' 구간(원기둥)을 두고 위/아래만 둥근 캡으로 마감한다.
    //  순수 타원체(달걀)는 옆면이 계속 휘어 둥글둥글하기만 한데, 가운데 직선
    //  구간을 넣으면 살짝 원기둥 같은 단단한 느낌이 난다. 캡 높이를 반경보다
    //  작게 두면(=납작한 돔) 더 원기둥처럼, 반경과 같으면 반구(=캡슐)가 된다.
    //
    //   rBot/rTop     : 아래/위 단면 반경 (다르면 테이퍼)
    //   length        : 전체 길이 (중심이 원점, ±length/2)
    //   capBot/capTop : 아래/위 둥근 캡의 세로 높이
    //--------------------------------------------------------------------------
    void renderRoundedColumn(float rBot, float rTop, float length,
                             float capBot, float capTop) {
        float cylH = length - capTop - capBot;
        if (cylH < 0.0f) cylH = 0.0f;

        // 가운데 곧은(테이퍼) 옆면
        if (cylH > 1e-4f)
            MeshUtils::renderTaperedEllipticCylinder(rBot, rBot, rTop, rTop, cylH);

        // 위 캡 (반구를 세로 capTop 으로 눌러 부드럽게)
        glPushMatrix();
        glTranslatef(0.0f, cylH * 0.5f, 0.0f);
        glScalef(1.0f, capTop / rTop, 1.0f);
        MeshUtils::renderHemisphere(rTop, 32, 16);
        glPopMatrix();

        // 아래 캡 (뒤집어서 볼록 아래로)
        glPushMatrix();
        glTranslatef(0.0f, -cylH * 0.5f, 0.0f);
        glScalef(1.0f, capBot / rBot, 1.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        MeshUtils::renderHemisphere(rBot, 32, 16);
        glPopMatrix();
    }

    // --- 리락쿠마 몸통 ---
    //  넓고 둥근 몸통에 '살짝 원기둥 느낌'을 주려고, 가운데 곧은 옆면 구간 +
    //  위/아래 둥근 캡(반경보다 낮은 납작 돔)으로 만든다. 최대 반경/전체 높이는
    //  기존 타원체와 같게 유지 → 배/지퍼/팔다리 부착 좌표는 그대로 맞는다.
    void renderBody() {
        Lighting::applyPlushMaterial();
        Palette::brown();
        const float rx = 1.15f, rz = 0.92f, halfH = 1.18f;
        const float cap = 1.1f;   // < rx → 약간 납작한 돔 = 원기둥 느낌
        glPushMatrix();
        glScalef(1.0f, 1.0f, rz / rx);                 // 앞뒤를 납작하게(타원 단면)
        renderRoundedColumn(rx, rx, 2.0f * halfH, cap, cap);
        glPopMatrix();
    }

    // --- 배 (흰 타원, 몸통 앞면에 납작하게) ---
    void renderBelly() {
        Lighting::applyPlushMaterial();
        Palette::belly();
        MeshUtils::renderEllipsoid(0.62f, 0.74f, 0.39f, 36, 32);
    }

    //--------------------------------------------------------------------------
    //  통통한 봉제 팔다리.  '살짝 원기둥 느낌'을 위해 가운데 곧은 구간 + 둥근
    //  끝으로 만든다(renderRoundedColumn). 어깨쪽(위)은 살짝 가늘고 끝(아래,
    //  손/발)은 통통하게 약간 테이퍼. 로컬 원점(0,0,0)을 어깨/고관절에 두고
    //  -y 로 length 만큼 내려온다.
    //
    //   bulkRX/RZ : 굵기(좌우/앞뒤 반경)   length : 팔다리 길이(원점→끝)
    //--------------------------------------------------------------------------
    void renderPlushLimb(float bulkRX, float bulkRZ, float length) {
        glPushMatrix();
        glScalef(1.0f, 1.0f, bulkRZ / bulkRX);         // 앞뒤 두께 보정
        glTranslatef(0.0f, -length * 0.5f, 0.0f);      // 위 끝이 원점(어깨)에 오게
        //  아래(끝): 반경 rx, 둥근 캡 / 위(어깨): 살짝 가늘고 납작한 캡
        renderRoundedColumn(bulkRX, bulkRX * 0.88f, length,
                            bulkRX * 0.92f, bulkRX * 0.78f);
        glPopMatrix();
    }

    // --- 팔 (좌우 공용. 좌우 차이는 어깨 회전이 담당) ---
    void renderArm() {
        Lighting::applyPlushMaterial();
        Palette::brown();
        renderPlushLimb(0.31f, 0.31f, 1.12f);

        // 손바닥(노란 패드) — 귀 안쪽 노란 부분과 같은 방식: 팔 '앞면'에 납작한
        //  타원을 살짝 돌출시켜 정면에서 동그란 패드가 그냥 보이게(눕히지 않음).
        //  z(앞으로 미는 양)가 클수록 더 튀어나온다.
        glPushMatrix();
        glTranslatef(0.0f, -0.9f, 0.2f);
        Palette::yellow();
        MeshUtils::renderEllipsoid(0.17f, 0.21f, 0.14f, 28, 20);
        glPopMatrix();
    }

    // --- 다리 (통통하고 긴 타원체 + 앞면 노란 발바닥 패드) ---
    void renderLeg() {
        Lighting::applyPlushMaterial();
        Palette::brown();
        renderPlushLimb(0.40f, 0.42f, 1.20f);

        // 노란 발바닥 — 발 끝 '아래쪽'에 깔아 발바닥(sole)처럼 보이게.
        //  y 를 더 내려 발끝에 붙이고, X 회전을 키워(53°) 패드가 아래를 향하게.
        //  → 발을 살짝 든 각도에서 정면-아래로 동그란 발바닥이 보인다.
        //  [조절] y(-1.14): 작을수록 발끝, z(0.25): 클수록 앞 돌출, 회전(53):
        //         클수록 더 아래(바닥)를 향함.
        glPushMatrix();
        glTranslatef(0.0f, -1.14f, 0.25f);
        glRotatef(53.0f, 1.0f, 0.0f, 0.0f);
        Palette::yellow();
        MeshUtils::renderEllipsoid(0.24f, 0.27f, 0.11f, 28, 20);
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

    // --- 흰 배 (몸통 앞면 아래-가운데. 갈색이 둘레로 보이게 적당한 크기) ---
    SceneNode* belly = new SceneNode();
    belly->setTranslation(0.0f, 0.05f, 0.65f);
    belly->setRenderFunction(renderBelly);
    torso->addChild(belly);

    // ===== 만세(별) 포즈: 양팔은 위로-바깥, 양다리는 아래로-바깥, 좌우 대칭 =====
    //  팔다리는 로컬에서 -Y 로 내려오므로 어깨/고관절에서 Z축으로 돌려 펼친다.
    //  Rz(θ) 적용 시 끝점 방향 = (sinθ, -cosθ). 왼쪽(-X)은 θ<0, 오른쪽(+X)은 θ>0
    //  으로 두면 좌우 대칭으로 벌어진다.

    // 왼팔: 위로-왼쪽 (Z -128°). 어깨를 몸 안쪽으로(±0.78) 깊이 넣어 팔 뿌리가
    //  몸통에 묻혀 '떨어져 보이는' 틈이 없게. X -8°로 손바닥 살짝 앞.
    SceneNode* leftShoulder = new SceneNode();
    leftShoulder->setTranslation(-0.78f, 0.52f, 0.12f);
    leftShoulder->setRotation(-128.0f, 0.0f, 0.0f, 1.0f);
    SceneNode* leftArmX = new SceneNode();
    leftArmX->setRotation(-8.0f, 1.0f, 0.0f, 0.0f);
    leftArmX->setRenderFunction(renderArm);
    leftShoulder->addChild(leftArmX);
    torso->addChild(leftShoulder);

    // 오른팔: 위로-오른쪽 (Z +128°). 좌우 대칭.
    SceneNode* rightShoulder = new SceneNode();
    rightShoulder->setTranslation(0.78f, 0.52f, 0.12f);
    rightShoulder->setRotation(128.0f, 0.0f, 0.0f, 1.0f);
    SceneNode* rightArmX = new SceneNode();
    rightArmX->setRotation(-8.0f, 1.0f, 0.0f, 0.0f);
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

    // --- 양다리: 만세에 맞춰 아래로-바깥으로 벌리되, 벌림을 줄이고(±22°) 고관절을
    //     몸 안쪽(±0.44)으로 넣어 다리 뿌리가 몸통에 묻혀 자연스럽게 이어지게. ---
    SceneNode* leftLegPivot = new SceneNode();
    leftLegPivot->setTranslation(-0.44f, -0.70f, 0.12f);
    leftLegPivot->setRotation(-22.0f, 0.0f, 0.0f, 1.0f);
    SceneNode* leftLegX = new SceneNode();
    leftLegX->setRotation(-12.0f, 1.0f, 0.0f, 0.0f);
    leftLegX->setRenderFunction(renderLeg);
    leftLegPivot->addChild(leftLegX);
    root->addChild(leftLegPivot);

    SceneNode* rightLegPivot = new SceneNode();
    rightLegPivot->setTranslation(0.44f, -0.70f, 0.12f);
    rightLegPivot->setRotation(22.0f, 0.0f, 0.0f, 1.0f);
    SceneNode* rightLegX = new SceneNode();
    rightLegX->setRotation(-12.0f, 1.0f, 0.0f, 0.0f);
    rightLegX->setRenderFunction(renderLeg);
    rightLegPivot->addChild(rightLegX);
    root->addChild(rightLegPivot);

    return root;
}