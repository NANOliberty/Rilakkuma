#pragma once
//==============================================================================
// TextureLoader.h  -  stb_image 기반 텍스처 로더
//==============================================================================
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>
#include <string>

namespace TextureLoader {

// 이미지 파일(png/bmp/jpg ...)을 OpenGL 2D 텍스처로 로드.
// 성공 시 텍스처 ID(>0)를 반환, 실패 시 0 을 반환한다(호출부에서 폴백 처리).
GLuint load2D(const std::string& path);

} // namespace TextureLoader
