//==============================================================================
// Body.cpp  -  몸통부 (B 담당)
//==============================================================================
//
//  ⚠️ 현재는 "최소 플레이스홀더"입니다. 프로젝트가 빌드/실행되도록 몸통+흰 배
//     정도만 그려 둡니다. B 가 아래 가이드를 따라 팔/다리/지퍼/꼬리를 채우면
//     됩니다. (A 가 만든 SceneNode / MeshUtils / Lighting / Palette 를 그대로
//     재사용하세요.)
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
    void renderBodyPlaceholder() {
        Lighting::applyPlushMaterial();
        Palette::brown();
        MeshUtils::renderEllipsoid(0.9f, 1.1f, 0.7f, 40, 40);
    }

    // --- 배 ---
    void renderBellyPlaceholder() {
        Lighting::applyPlushMaterial();
        Palette::belly();
        MeshUtils::renderEllipsoid(0.55f, 0.65f, 0.30f, 32, 32);
    }

    // --- 왼쪽 팔 ---
    void renderLeftArm() {
        Lighting::applyPlushMaterial();
        Palette::brown();

        glPushMatrix();
        glTranslatef(0.0f, -0.55f, 0.0f);
        MeshUtils::renderCapsule(0.27f, 1.1f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, -1.1f, 0.25f);
        glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
        Palette::yellow();
        MeshUtils::renderEllipsoid(0.15f, 0.18f, 0.05f, 16, 16);
        glPopMatrix();
    }

    // --- 오른쪽 팔 ---
    void renderRightArm() {
        Lighting::applyPlushMaterial();
        Palette::brown();

        glPushMatrix();
        glTranslatef(0.0f, -0.55f, 0.0f);
        MeshUtils::renderCapsule(0.27f, 1.1f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, -1.1f, -0.25f);   
        glRotatef(-10.0f, 1.0f, 0.0f, 0.0f); 
        Palette::yellow();
        MeshUtils::renderEllipsoid(0.15f, 0.18f, 0.05f, 16, 16); 
        glPopMatrix();
    }

    // --- 다리 (노란 발바닥 통합) ---
    void renderLeg() {
        Lighting::applyPlushMaterial();
        Palette::brown();

        glPushMatrix();
        glTranslatef(0.0f, -0.45f, 0.0f);
        MeshUtils::renderCapsule(0.37f, 1.1f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, -1.25f, 0.0f);
        Palette::yellow();
        MeshUtils::renderEllipsoid(0.25f, 0.15f, 0.30f, 16, 16);
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
            float zPos = -0.69f + zCurveOffset;

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

        float sliderZ = -0.69f + ((sliderY * sliderY) * 0.30f) - 0.04f;
        glTranslatef(0.0f, sliderY, sliderZ);

        glRotatef(30.0f, 1.0f, 0.0f, 0.0f);

        glutSolidCube(0.07f);

        glTranslatef(0.0f, -0.07f, 0.0f);
        glutSolidTorus(0.015f, 0.05f, 10, 16);
        glPopMatrix();
    }

} // namespace

SceneNode* BuildBody() {

    // 하체 노드
    SceneNode* root = new SceneNode();

    // 상체 노드
    SceneNode* torso = new SceneNode();
    torso->setRenderFunction(renderBodyPlaceholder);
    torso->setRotation(-15.0f, 0.0f, 0.0f, 1.0f);
    root->addChild(torso);

    // --- 흰 배 (상체에 부착) ---
    SceneNode* belly = new SceneNode();
    belly->setTranslation(0.0f, -0.1f, 0.50f);
    belly->setRenderFunction(renderBellyPlaceholder);
    torso->addChild(belly);

    // --- 왼쪽 팔 조립 (상체에 부착되어 같이 회전) ---
    SceneNode* leftShoulder = new SceneNode();
    leftShoulder->setTranslation(-0.9f, 0.7f, 0.0f);
    leftShoulder->setRotation(-140.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* leftArmX = new SceneNode();
    leftArmX->setRotation(-20.0f, 1.0f, 0.0f, 0.0f);
    leftArmX->setRenderFunction(renderLeftArm);
    leftShoulder->addChild(leftArmX);
    torso->addChild(leftShoulder);

    // --- 오른쪽 팔 조립 (상체에 부착되어 같이 회전) ---
    SceneNode* rightShoulder = new SceneNode();
    rightShoulder->setTranslation(0.9f, 0.7f, 0.0f);
    rightShoulder->setRotation(-50.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* rightArmX = new SceneNode();
    rightArmX->setRotation(-70.0f, 1.0f, 0.0f, 0.0f);
    rightArmX->setRenderFunction(renderRightArm);
    rightShoulder->addChild(rightArmX);
    torso->addChild(rightShoulder);

    // --- 꼬리 조립 (상체에 부착) ---
    SceneNode* tail = new SceneNode();
    tail->setTranslation(0.0f, -0.5f, -0.74f);
    tail->setRenderFunction([]() {
        Lighting::applyPlushMaterial();
        Palette::brown(); 
        glutSolidSphere(0.22f, 24, 24);
        });
    torso->addChild(tail);

    // --- 등 지퍼 조립 (상체에 부착) ---
    SceneNode* zipper = new SceneNode();
    zipper->setRenderFunction(renderZipper);
    torso->addChild(zipper);

    // =========================================================================
    // 하체 조립: 지면 수평 유지를 위해 상체(torso)가 아닌 최상위(root)에 직속 연결
    // =========================================================================

    // --- 왼쪽 다리 ---
    SceneNode* leftLegPivot = new SceneNode();
    leftLegPivot->setTranslation(-0.55f, -0.7f, 0.15f);
    leftLegPivot->setRotation(-40.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* leftLegMesh = new SceneNode();
    leftLegMesh->setRenderFunction(renderLeg);
    leftLegPivot->addChild(leftLegMesh);
    root->addChild(leftLegPivot);

    // --- 오른쪽 다리 ---
    SceneNode* rightLegPivot = new SceneNode();
    rightLegPivot->setTranslation(0.55f, -0.7f, 0.15f);
    rightLegPivot->setRotation(40.0f, 0.0f, 0.0f, 1.0f);

    SceneNode* rightLegMesh = new SceneNode();
    rightLegMesh->setRenderFunction(renderLeg);
    rightLegPivot->addChild(rightLegMesh);
    root->addChild(rightLegPivot);

    return root;
}