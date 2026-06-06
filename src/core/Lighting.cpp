#include "Lighting.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>
#include <vector>

namespace Lighting {

namespace {
GLuint g_furTex = 0;
const float kFurFreq = 4.0f;   // 단위 길이당 털결 반복 수(클수록 결이 곱다)
} // namespace

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

//------------------------------------------------------------------------------
// 절차적 털결 텍스처: 세로 방향으로 부드럽게 이어지는 미세 노이즈(휘도).
//  밝기를 0.80~1.0 의 좁은 범위로 압축해, MODULATE 곱했을 때 표면색이 살짝
//  밝고 어둡게 흔들리는 '결' 정도만 생긴다(과하지 않게).
//------------------------------------------------------------------------------
void initFur() {
    const int N = 128;
    std::vector<float> n(N * N);

    // 1) 의사난수 노이즈
    unsigned int seed = 1234567u;
    auto rnd = [&]() {
        seed = seed * 1103515245u + 12345u;
        return ((seed >> 16) & 0x7fff) / 32767.0f;   // 0..1
    };
    for (int i = 0; i < N * N; ++i) n[i] = rnd();

    // 2) 세로(y)로 여러 번 블러 → 위아래로 이어지는 털결 streak
    std::vector<float> m(N * N);
    for (int pass = 0; pass < 4; ++pass) {
        for (int x = 0; x < N; ++x)
            for (int y = 0; y < N; ++y) {
                int yu = (y - 1 + N) % N, yd = (y + 1) % N;
                m[y * N + x] = (n[y * N + x] + n[yu * N + x] + n[yd * N + x]) / 3.0f;
            }
        n.swap(m);
    }

    // 3) 0..1 정규화 후 [0.80,1.0] 휘도로 압축
    float lo = 1e9f, hi = -1e9f;
    for (float v : n) { lo = (v < lo ? v : lo); hi = (v > hi ? v : hi); }
    float inv = (hi > lo) ? 1.0f / (hi - lo) : 1.0f;

    std::vector<unsigned char> tex(N * N);
    for (int i = 0; i < N * N; ++i) {
        float v = (n[i] - lo) * inv;            // 0..1
        float lum = 0.80f + 0.20f * v;          // 0.80..1.0
        tex[i] = (unsigned char)(lum * 255.0f);
    }

    glGenTextures(1, &g_furTex);
    glBindTexture(GL_TEXTURE_2D, g_furTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, N, N, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, tex.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void beginFur() {
    if (g_furTex == 0) return;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_furTex);
    // MODULATE: 텍스처 휘도 × 부위색(Palette). 색은 호출부에서 이미 설정돼 있으니
    //  여기서 glColor 를 건드리지 않는다(덮으면 부위색이 사라짐).
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // 오브젝트 좌표 기반 자동 UV(메쉬에 UV 가 없어도 적용됨)
    GLfloat sPlane[4] = { kFurFreq, 0.0f, 0.0f, 0.0f };
    GLfloat tPlane[4] = { 0.0f, kFurFreq, 0.0f, 0.0f };
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sPlane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, tPlane);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
}

void endFur() {
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

} // namespace Lighting
