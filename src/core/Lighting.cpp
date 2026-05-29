#include "Lighting.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>

namespace Lighting {

void init() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);   // 스케일된 메쉬 조명 보정 (필수)

    // glColor* 가 ambient+diffuse 머티리얼로 매핑되도록
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // 인형 질감: ambient/diffuse 높임, specular 낮춤
    GLfloat ambient[]  = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat diffuse[]  = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat position[] = {5.0f, 10.0f, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    // 전역 ambient 도 살짝 올려 전체적으로 부드럽게
    GLfloat globalAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
}

void applyPlushMaterial() {
    // 인형이므로 거의 무광. specular 낮고 shininess 작게.
    GLfloat matSpecular[]  = {0.08f, 0.08f, 0.08f, 1.0f};
    GLfloat matShininess[] = {6.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
}

} // namespace Lighting
