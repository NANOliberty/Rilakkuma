#include "InputHandler.h"
#include "Camera.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>
#include <cstdlib>

namespace {
Camera* g_camera = nullptr;

bool g_dragging = false;
int  g_lastX = 0;
int  g_lastY = 0;

const float kRotSpeed = 0.4f;   // 픽셀당 회전 각도(도)
const float kZoomIn   = 0.9f;   // 'a' : 거리 0.9배 (가까워짐)
const float kZoomOut  = 1.1f;   // 'z' : 거리 1.1배 (멀어짐)
} // namespace

namespace InputHandler {

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            g_dragging = true;
            g_lastX = x;
            g_lastY = y;
        } else {
            g_dragging = false;
        }
    }
}

void mouseMotion(int x, int y) {
    if (!g_dragging || !g_camera) return;

    int dx = x - g_lastX;
    int dy = y - g_lastY;
    g_lastX = x;
    g_lastY = y;

    // 가로 드래그 -> theta, 세로 드래그 -> phi
    g_camera->addOrbit(-dx * kRotSpeed, dy * kRotSpeed);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int /*x*/, int /*y*/) {
    if (!g_camera) return;
    switch (key) {
        case 'a': case 'A':
            g_camera->zoom(kZoomIn);   // zoom-in
            glutPostRedisplay();
            break;
        case 'z': case 'Z':
            g_camera->zoom(kZoomOut);  // zoom-out
            glutPostRedisplay();
            break;
        case 27: // ESC
            std::exit(0);
            break;
        default:
            break;
    }
}

void init(Camera* camera) {
    g_camera = camera;
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
}

} // namespace InputHandler
