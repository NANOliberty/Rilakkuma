#pragma once
//==============================================================================
// Lighting.h  -  조명 설정 (GL_LIGHT0). 인형 질감: ambient/diffuse 높이고
//                specular 낮춤.
//==============================================================================

namespace Lighting {

// GL_LIGHTING / GL_LIGHT0 활성화 + 광원 파라미터 설정.
void init();

// 인형(플러시) 느낌의 머티리얼 specular/shininess 설정.
// 각 부위 렌더 직전에 호출하면 광택을 억제한다.
void applyPlushMaterial();

} // namespace Lighting
