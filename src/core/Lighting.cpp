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

    // 인형 질감: ambient/diffuse 위주, specular 없음.
    //  중요: (전역ambient + light ambient) + light diffuse <= 1.0 으로 맞춘다.
    //  이렇게 하면 라이팅 결과가 1.0 을 넘어 clamp 되지 않아, 단색 머리와
    //  텍스처(MODULATE) 얼굴 패치의 밝기가 정확히 일치한다(경계 사라짐).
    GLfloat ambient[]  = {0.25f, 0.25f, 0.25f, 1.0f};
    GLfloat diffuse[]  = {0.50f, 0.50f, 0.50f, 1.0f};
    GLfloat specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat position[] = {5.0f, 10.0f, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    // 전역 ambient. (0.25 + light ambient 0.25) + diffuse 0.50 = 1.0
    GLfloat globalAmbient[] = {0.25f, 0.25f, 0.25f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
}

void applyPlushMaterial() {
    // 인형이므로 완전 무광(specular 0).
    //  specular 가 있으면, 단색 머리와 텍스처(MODULATE) 얼굴 패치 사이에서
    //  specular 가 텍스처에 곱해지는지 여부가 달라 패치 경계가 음영 차이로
    //  드러난다. 0 으로 두면 머리와 패치가 픽셀 단위로 동일해져 경계가 사라진다.
    GLfloat matSpecular[]  = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat matShininess[] = {0.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
}

} // namespace Lighting
