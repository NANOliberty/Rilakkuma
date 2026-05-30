# B 파트 구현 가이드 (몸통부 + 배경 + 텍스처 매핑)

> A 가 만든 공통 인프라(SceneNode / MeshUtils / Lighting / Palette / TextureLoader)는
> 이미 동작합니다. B 는 `src/character/Body.cpp` 와 `src/scene/Background.cpp` 만
> 채우면 됩니다. 아래 스니펫을 거의 복붙 수준으로 쓸 수 있게 정리했습니다.

---

## 0. 시작 전에

- 빌드/실행이 되는지 먼저 확인 (docs/WINDOWS_SETUP.md). 현재 Body 는
  **몸통 + 흰 배만** 그려지는 플레이스홀더 상태입니다. 머리는 완성돼 있습니다.
- 작업은 `feature/body-arms-pose` 같은 브랜치에서 하고 자주 push.
- 색은 항상 `Palette::`, 각 부위 렌더 직전 `Lighting::applyPlushMaterial()`.

## 1. 사용 가능한 메쉬 (이미 구현됨, `core/MeshUtils.h`)

```cpp
MeshUtils::renderEllipsoid(rx, ry, rz);                 // 단색 타원체
MeshUtils::renderCapsule(radius, height);               // 둥근 팔다리
MeshUtils::renderTaperedEllipticCylinder(bM,bm, tM,tm, h); // 위아래 다른 굵기
// + glutSolidSphere / glutSolidTorus / glutSolidCone 도 자유롭게
```

## 2. 팔 — 인사 포즈의 핵심

팔 메쉬는 **위쪽 끝이 로컬 원점**에 오도록 그린다(아래로 길게). 그래야 어깨가
회전 중심이 된다. Z축 회전 + X축 회전 두 개를 노드 중첩으로 합성한다.

```cpp
// Body.cpp 안에 추가
static void renderArm() {
    Lighting::applyPlushMaterial();
    Palette::brown();
    glPushMatrix();
    glTranslatef(0.0f, -0.6f, 0.0f);   // 캡슐 중심을 아래로 -> 위끝이 원점
    MeshUtils::renderCapsule(0.34f, 0.7f);
    glPopMatrix();

    // 손바닥 노란 타원 (팔 끝 = 아래쪽)
    glPushMatrix();
    glTranslatef(0.0f, -1.25f, 0.0f);
    Palette::yellow();
    MeshUtils::renderEllipsoid(0.3f, 0.18f, 0.3f);
    glPopMatrix();
}
```

```cpp
// BuildBody() 안에서 — 왼팔(번쩍 든 팔): Z+130, X-15
SceneNode* lShoulder = new SceneNode();
lShoulder->setTranslation(-1.2f, 0.5f, 0.0f);
lShoulder->setRotation(130.0f, 0,0,1);
SceneNode* lArmX = new SceneNode();
lArmX->setRotation(-15.0f, 1,0,0);
lArmX->setRenderFunction(renderArm);
lShoulder->addChild(lArmX);
body->addChild(lShoulder);

// 오른팔(배쪽): Z-25, X+25
SceneNode* rShoulder = new SceneNode();
rShoulder->setTranslation(1.2f, 0.5f, 0.0f);
rShoulder->setRotation(-25.0f, 0,0,1);
SceneNode* rArmX = new SceneNode();
rArmX->setRotation(25.0f, 1,0,0);
rArmX->setRenderFunction(renderArm);
rShoulder->addChild(rArmX);
body->addChild(rShoulder);
```

## 3. 다리

```cpp
static void renderLeg() {
    Lighting::applyPlushMaterial();
    Palette::brown();
    glPushMatrix();
    glTranslatef(0.0f, -0.45f, 0.0f);
    MeshUtils::renderCapsule(0.38f, 0.5f);
    glPopMatrix();
    // 발바닥 노란 타원
    glPushMatrix();
    glTranslatef(0.0f, -0.95f, 0.1f);
    Palette::yellow();
    MeshUtils::renderEllipsoid(0.34f, 0.2f, 0.42f);
    glPopMatrix();
}

// BuildBody(): 왼다리 Z-10 / 오른다리 Z+10
SceneNode* lLeg = new SceneNode();
lLeg->setTranslation(-0.6f, -1.2f, 0.0f);
lLeg->setRotation(-10.0f, 0,0,1);
lLeg->setRenderFunction(renderLeg);
body->addChild(lLeg);

SceneNode* rLeg = new SceneNode();
rLeg->setTranslation(0.6f, -1.2f, 0.0f);
rLeg->setRotation(10.0f, 0,0,1);
rLeg->setRenderFunction(renderLeg);
body->addChild(rLeg);
```

## 4. 꼬리 + 등 지퍼

```cpp
// 꼬리: 작은 갈색 구, 뒤쪽
SceneNode* tail = new SceneNode();
tail->setTranslation(0.0f, 0.0f, -1.4f);
tail->setRenderFunction([](){
    Lighting::applyPlushMaterial(); Palette::brown();
    glutSolidSphere(0.28, 24, 24);
});
body->addChild(tail);

// 등 지퍼: 등(−z)에 작은 토러스를 세로로 반복
static void renderZipper() {
    Lighting::applyPlushMaterial();
    Palette::zipper();
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glTranslatef(0.0f, 0.9f - i * 0.25f, -1.28f);
        glRotatef(90.0f, 1,0,0);
        glutSolidTorus(0.025, 0.07, 10, 16);
        glPopMatrix();
    }
}
SceneNode* zipper = new SceneNode();
zipper->setRenderFunction(renderZipper);
body->addChild(zipper);
```

> 좌표는 등 곡면에 맞게 z 값을 −1.2 ~ −1.3 사이로 미세 조정하세요.

## 5. 배경 (`scene/Background.cpp`)

현재 연한 바닥 평면 플레이스홀더가 있습니다. 선택지:
- **단색**: `main.cpp` 의 `glClearColor` 만으로 충분.
- **바닥+벽(방 안)**: 큰 박스 안쪽 면. 텍스처 쓰려면 `TextureLoader::load2D` 사용.
- 텍스처를 추가하면 `assets/textures/` 에 넣고 A 의 로더로 로드.

## 6. 통합 시 주의

- A 가 만지는 파일: `core/*`, `Head.cpp`, `main.cpp`.
- B 가 만지는 파일: `Body.cpp`, `Background.cpp`, (텍스처 추가 시) assets.
- 공통 파일(`Rilakkuma.h/.cpp`, `MeshUtils.*`)은 바꿀 때 서로 알리기.
- 작은 커밋 + 자주 push 로 머지 충돌 최소화.
