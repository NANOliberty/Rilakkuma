#include "TextureLoader.h"
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// stb_image 구현부는 이 한 곳에서만 펼친다.
#define STB_IMAGE_IMPLEMENTATION
#include "../thirdparty/stb_image.h"

#include <cstdio>

namespace TextureLoader {

GLuint load2D(const std::string& path) {
    int w = 0, h = 0, channels = 0;

    // OpenGL 텍스처 좌표는 좌하단 원점이므로 세로 뒤집어 로드
    stbi_set_flip_vertically_on_load(1);

    // 4채널(RGBA)로 강제 로드 -> 포맷 분기 단순화
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
    if (!data) {
        std::printf("[TextureLoader] FAILED to load '%s' (%s)\n",
                    path.c_str(), stbi_failure_reason());
        return 0;
    }

    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // freeglut 환경에서 안전한 밉맵 생성 (GLU 사용)
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h,
                      GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    std::printf("[TextureLoader] loaded '%s' (%dx%d, %d ch) -> tex %u\n",
                path.c_str(), w, h, channels, texId);
    return texId;
}

} // namespace TextureLoader
