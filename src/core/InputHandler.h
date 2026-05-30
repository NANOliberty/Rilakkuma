#pragma once
//==============================================================================
// InputHandler.h  -  GLUT 마우스/키보드 콜백.
//   * 좌클릭 드래그  -> 3D 회전 (Camera orbit)
//   * 'a' 키         -> zoom-in
//   * 'z' 키         -> zoom-out
//   * ESC            -> 종료
//==============================================================================
class Camera;

namespace InputHandler {

// 조작 대상 카메라를 연결하고 GLUT 콜백을 등록한다.
void init(Camera* camera);

// 아래 콜백들은 glut*Func 에 등록되거나 init() 내부에서 등록된다.
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);

} // namespace InputHandler
