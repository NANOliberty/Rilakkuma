//==============================================================================
// Background.cpp  -  배경 (B 담당)
//==============================================================================
//
//  ⚠️ 플레이스홀더: 캐릭터가 떠 보이지 않도록 간단한 바닥 평면만 깔아 둡니다.
//     B 가 단색 / 바닥 / 방 안 중 자유롭게 결정해 확장하세요(PROJECT_SPEC 4.2).
//
//  아이디어:
//    - 단색 배경: glClearColor 로 충분 (main 에서 설정 중)
//    - 바닥 평면: 큰 사각형 + (선택) 체크무늬/나무 텍스처
//    - 방 안: 바닥 + 뒷벽 + 옆벽 (큰 박스 안쪽), 창문/액자 등 prop
//    - 텍스처가 필요하면 A 의 TextureLoader::load2D 를 그대로 사용
//==============================================================================
#include "Background.h"
#include "../core/SceneNode.h"
#include "../core/Lighting.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>

namespace {

void renderFloorPlaceholder() {
    Lighting::applyPlushMaterial();
    glColor3f(0.85f, 0.88f, 0.92f); // 연한 회청색 바닥

    const float s = 12.0f;   // 평면 반폭
    const float y = -2.4f;   // 발 아래
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-s, y, -s);
    glVertex3f(-s, y,  s);
    glVertex3f( s, y,  s);
    glVertex3f( s, y, -s);
    glEnd();
}

} // namespace

SceneNode* BuildBackground() {
    SceneNode* bg = new SceneNode();
    bg->setRenderFunction(renderFloorPlaceholder);
    return bg;
}
