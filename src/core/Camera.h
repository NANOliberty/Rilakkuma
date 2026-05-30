#pragma once
//==============================================================================
// Camera.h  -  궤도(orbit) 카메라. 캐릭터를 중심으로 theta/phi 회전하고
//              distance 로 줌인/줌아웃 한다.
//==============================================================================

class Camera {
public:
    Camera();

    // gluLookAt 호출. display() 의 모델뷰 세팅 단계에서 부른다.
    void apply() const;

    // 마우스 드래그 누적 회전(도 단위 델타).
    void addOrbit(float dTheta, float dPhi);

    // 줌. factor < 1 이면 가까워지고(zoom-in), > 1 이면 멀어진다(zoom-out).
    void zoom(float factor);

    void setTarget(float x, float y, float z);

private:
    float theta;     // 수평 각(도) — y축 기준
    float phi;       // 수직 각(도) — 0=수평, +면 위에서 내려봄
    float distance;  // 타겟까지 거리
    float tx, ty, tz; // 바라보는 타겟

    float minDist, maxDist;
};
