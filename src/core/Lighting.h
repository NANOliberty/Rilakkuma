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

// 털결(fur) 텍스처를 절차적으로 생성한다. GL 컨텍스트 생성 후 init() 다음에 1회.
void initFur();

// 갈색 인형 부위 렌더 직전/직후에 호출. 은은한 털결 노이즈를 표면에 입힌다.
//  glTexGen(오브젝트 좌표)으로 UV 없는 메쉬에도 자동 적용된다.
//  begin~end 사이의 메쉬만 영향 받는다(눈/지퍼/배/패드 등은 감싸지 않으면 매끈).
void beginFur();
void endFur();

} // namespace Lighting
