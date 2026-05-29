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

// --- 플레이스홀더: 몸통(갈색 타원체) ---
void renderBodyPlaceholder() {
    Lighting::applyPlushMaterial();
    Palette::brown();
    // 몸통: 머리보다 약간 큰 타원체 (살짝 세로로)
    MeshUtils::renderEllipsoid(1.35f, 1.45f, 1.25f, 40, 40);
}

// --- 플레이스홀더: 흰 배 (앞쪽 +z 로 살짝 돌출) ---
void renderBellyPlaceholder() {
    Lighting::applyPlushMaterial();
    Palette::belly();
    MeshUtils::renderEllipsoid(0.8f, 0.9f, 0.5f, 32, 32);
}

} // namespace

SceneNode* BuildBody() {
    SceneNode* body = new SceneNode();
    body->setRenderFunction(renderBodyPlaceholder);

    // 흰 배 (앞면)
    SceneNode* belly = new SceneNode();
    belly->setTranslation(0.0f, -0.05f, 0.95f);
    belly->setRenderFunction(renderBellyPlaceholder);
    body->addChild(belly);

    // TODO(B): 아래를 채우세요 —
    //   - 왼팔(번쩍 든 팔)  : 어깨(-1.2,0.5,0) + Z130°, X-15°
    //   - 오른팔(배쪽 팔)   : 어깨(+1.2,0.5,0) + Z-25°, X+25°
    //   - 왼/오른 다리      : (∓0.6,-1.2,0) + Z∓10°, 발바닥 노란 타원
    //   - 손/발바닥 노란 타원
    //   - 등 지퍼(토러스 반복) + 꼬리(작은 구, z=-1.4)
    //   docs/B_GUIDE.md 의 코드 스니펫을 참고하면 바로 붙일 수 있습니다.

    return body;
}
