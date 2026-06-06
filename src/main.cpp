//==============================================================================
// main.cpp  -  진입점. GLUT 초기화, 콜백 등록, 씬 빌드/렌더.
//==============================================================================
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>
#include <cstdio>

#include "core/SceneNode.h"
#include "core/Camera.h"
#include "core/InputHandler.h"
#include "core/Lighting.h"
#include "core/TextureLoader.h"
#include "character/Rilakkuma.h"
#include "scene/Background.h"

namespace {
Camera     g_camera;
SceneNode* g_rilakkuma  = nullptr;
SceneNode* g_background  = nullptr;

int g_winW = 1000;
int g_winH = 750;

// 작업 디렉터리가 어디든(빌드 산출물 옆 / 프로젝트 루트) 찾도록 후보 경로 시도.
GLuint loadFaceTexture() {
    const char* candidates[] = {
        "assets/textures/face_cream.png",
        "../assets/textures/face_cream.png",
        "../../assets/textures/face_cream.png",
    };
    for (const char* path : candidates) {
        GLuint id = TextureLoader::load2D(path);
        if (id != 0) return id;
    }
    std::printf("[main] face_cream.png not found — cream 영역을 단색으로 표시합니다.\n");
    return 0;
}

void init() {
    glClearColor(0.93f, 0.95f, 0.98f, 1.0f); // 밝은 배경

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);   // 스케일된 메쉬 조명 보정 (필수)
    glShadeModel(GL_SMOOTH);

    Lighting::init();

    GLuint faceTex = loadFaceTexture();
    SetFaceTexture(faceTex);

    g_background = BuildBackground();
    g_rilakkuma  = BuildRilakkuma();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    g_camera.apply();

    // 배경(방)만 백페이스 컬링으로 그린다. 카메라가 어떤 벽의 '바깥'에서 안을
    // 보면 그 벽은 뒷면이 되어 컬링 → 자동으로 투과되어 가운데 캐릭터가 안 가린다.
    // (세 벽 모두 안쪽 면이 front-face(CCW)라 안에서는 정상으로 보인다.)
    if (g_background) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        g_background->render();
        glDisable(GL_CULL_FACE);   // 캐릭터는 컬링 영향 없이 그대로 그림
    }
    if (g_rilakkuma)  g_rilakkuma->render();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    g_winW = w;
    g_winH = (h == 0) ? 1 : h;

    glViewport(0, 0, g_winW, g_winH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)g_winW / (double)g_winH, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

} // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(g_winW, g_winH);
    glutCreateWindow("Rilakkuma - 3D Virtual Human (CG Final Project)");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    InputHandler::init(&g_camera); // 마우스 드래그 회전 + a/z 줌

    std::printf("=== Rilakkuma ===\n");
    std::printf("  마우스 좌클릭 드래그 : 3D 회전\n");
    std::printf("  a : zoom-in / z : zoom-out / ESC : 종료\n");

    glutMainLoop();
    return 0;
}
