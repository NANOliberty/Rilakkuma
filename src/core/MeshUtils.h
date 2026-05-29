#pragma once
//==============================================================================
// MeshUtils.h  -  수학식으로 직접 구현한 공통 메쉬 함수들
//
//  과제 규칙상 "미리 제작된 메쉬 파일 로드"는 금지이므로, 아래 메쉬들은 모두
//  파라메트릭 수식으로 정점/법선/텍스처 좌표를 계산해 즉석에서 그린다.
//  (glutSolidSphere 등 기본 primitive 와 함께 사용)
//
//  모든 메쉬는 "원점 기준"으로 그려진다. 위치/회전/스케일은 SceneNode 가 담당.
//==============================================================================

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>

namespace MeshUtils {

// 위쪽 반구(돔). 평평한 단면이 y=0 평면, 볼록한 쪽이 +y.
void renderHemisphere(float radius, int slices = 32, int stacks = 16);

// 위/아래 단면이 서로 다른 타원인 실린더(팔다리·몸통 연결용).
//  - bottom* : y = -height/2 단면의 장/단반경
//  - top*    : y = +height/2 단면의 장/단반경
//  옆면 + 위/아래 뚜껑까지 그린다.
void renderTaperedEllipticCylinder(
    float bottomMajor, float bottomMinor,
    float topMajor, float topMinor,
    float height, int slices = 32, int stacks = 16);

// 캡슐: 가운데 실린더 + 양 끝 반구. 팔다리를 둥글게 마감할 때.
//  전체 길이 = height + 2*radius, 중심은 원점, 축은 y.
void renderCapsule(float radius, float height, int slices = 32, int stacks = 16);

// 텍스처가 매핑된 타원체(얼굴 cream 영역용).
//  rx/ry/rz 반경의 타원체에 구면 UV 를 자동 부여하고 texId 를 입힌다.
//  texId == 0 이면 텍스처 없이 흰색 단색으로 그린다(폴백).
void renderTexturedEllipsoid(float rx, float ry, float rz, GLuint texId,
                             int slices = 32, int stacks = 16);

// 단색 타원체(흰 배, 손/발바닥 노란 타원 등). 텍스처 없이.
void renderEllipsoid(float rx, float ry, float rz, int slices = 32, int stacks = 16);

} // namespace MeshUtils
