#pragma once
//==============================================================================
// Rilakkuma.h  -  캐릭터 루트 빌더 + 공용 컬러 팔레트.
//
//  BuildRilakkuma() 가 머리(Head) + 몸(Body) 를 자식으로 가진 루트 SceneNode 를
//  만들어 돌려준다. main 은 이 루트를 받아 매 프레임 render() 만 호출하면 된다.
//==============================================================================

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>

class SceneNode;

// PROJECT_SPEC 5.4 컬러 팔레트 (RGB 0~1)
namespace Palette {
inline void brown()  { glColor3f(0.761f, 0.502f, 0.239f); } // 머리/몸/팔다리 (원본 황갈색)
inline void cream()  { glColor3f(0.96f, 0.92f, 0.85f); } // 귀/얼굴 cream 배경
inline void yellow() { glColor3f(0.98f, 0.85f, 0.40f); } // 귀 안쪽, 손/발바닥
inline void belly()  { glColor3f(0.95f, 0.93f, 0.88f); } // 흰 배
inline void eye()    { glColor3f(0.10f, 0.08f, 0.08f); } // 눈
inline void zipper() { glColor3f(0.70f, 0.70f, 0.72f); } // 지퍼 금속
} // namespace Palette

// 얼굴 cream 영역에 매핑할 텍스처 ID 를 등록한다(0 이면 단색 폴백).
// main 에서 텍스처 로드 후, BuildRilakkuma 호출 전에 세팅한다.
void SetFaceTexture(GLuint texId);

// 리락쿠마 전체(머리+몸) 루트 노드 생성.
SceneNode* BuildRilakkuma();

// 파트별 빌더 (Head.cpp / Body.cpp 에 구현)
SceneNode* BuildHead();
SceneNode* BuildBody();
