#include "Rilakkuma.h"
#include "../core/SceneNode.h"

// 얼굴 텍스처 ID 는 Head.cpp 에서 cream 영역 렌더 시 참조한다.
// 여기서 정의하고 Head.cpp 가 extern 으로 가져다 쓴다.
GLuint g_faceTexId = 0;

void SetFaceTexture(GLuint texId) {
    g_faceTexId = texId;
}

SceneNode* BuildRilakkuma() {
    SceneNode* root = new SceneNode();
    // 캐릭터 전체는 원점 기준. 필요 시 여기서 전체 위치/스케일 조정 가능.
    root->addChild(BuildHead());  // Head.cpp  (A 담당)
    root->addChild(BuildBody());  // Body.cpp  (B 담당)
    return root;
}
