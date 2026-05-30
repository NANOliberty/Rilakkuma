// Offscreen preview harness (개발 검증용, 제출물 아님).
// Xvfb 위에서 한 프레임 렌더 후 PPM(P6)로 덤프하고 종료한다.
// usage: preview <thetaDeg> <phiDeg> <dist> <out.ppm>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "../src/core/SceneNode.h"
#include "../src/core/Camera.h"
#include "../src/core/Lighting.h"
#include "../src/core/TextureLoader.h"
#include "../src/character/Rilakkuma.h"
#include "../src/scene/Background.h"

static int W = 520, H = 620;

int main(int argc, char** argv) {
    float theta = argc > 1 ? atof(argv[1]) : 0.0f;
    float phi   = argc > 2 ? atof(argv[2]) : 15.0f;
    float dist  = argc > 3 ? atof(argv[3]) : 8.0f;
    const char* out = argc > 4 ? argv[4] : "preview.ppm";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(W, H);
    glutCreateWindow("preview");

    glClearColor(0.93f, 0.95f, 0.98f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    Lighting::init();

    GLuint tex = TextureLoader::load2D("assets/textures/face_cream.png");
    SetFaceTexture(tex);
    SceneNode* bg   = BuildBackground();
    SceneNode* bear = BuildRilakkuma();

    glViewport(0, 0, W, H);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)W / H, 0.1, 100.0);

    Camera cam;
    cam.addOrbit(theta, phi - 15.0f); // 기본 phi=15 기준 보정
    cam.zoom(dist / 8.0f);            // 기본 dist=8 기준 보정

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.apply();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bg->render();
    bear->render();
    glFinish();

    std::vector<unsigned char> px(W * H * 3);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, px.data());

    // PPM 은 위->아래, GL 은 아래->위 이므로 뒤집어 저장
    FILE* f = fopen(out, "wb");
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (int y = H - 1; y >= 0; --y)
        fwrite(&px[y * W * 3], 1, W * 3, f);
    fclose(f);
    std::printf("wrote %s\n", out);
    return 0;
}
