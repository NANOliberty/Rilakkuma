# 리락쿠마 3D 모델링 프로젝트 명세서 v2

> **컴퓨터 그래픽스 기말 프로젝트** — 2인 팀, OpenGL 기반 3D Virtual Human Modeling & Visualization
>
> v2 변경사항: 포즈 확정 (인사 포즈), 배경 담당자 명시, 얼굴 텍스처 방식 결정

---

## 1. 프로젝트 개요

### 1.1 과제 요구사항 (교수 명세)

- OpenGL primitive 함수와 간단히 수학식으로 모델링 가능한 메쉬를 사용하여 **3D Virtual Human을 사실적으로 모델링**
- **텍스처 매핑** 등으로 실감나게 visualization
- **마우스 드래그**를 이용한 3D rotation
- **`a` 키 zoom-in, `z` 키 zoom-out**
- 모든 모델링 요소는 **3D 객체**여야 함
- 모든 요소는 **원점 기준 좌표계**로 만들고, `glTranslatef`/`glRotatef`로 위치 조정
- 텍스처 파일은 자유
- **❌ 미리 제작된 메쉬 파일 로드 금지**
- **제출물**: Visual Studio 2015 이상 프로젝트 (.cpp, .h) + PPT + 시연 동영상
- **마감**: 6월 13일(토), `class.ssu.ac.kr` 온라인 제출
- 팀 별 1명만 제출, PPT 첫 장에 팀원 학번/이름 명시

### 1.2 사용 가능한 OpenGL primitive

```cpp
void glutSolidCube(GLdouble size);
void glutSolidSphere(GLdouble radius, GLint slices, GLint stacks);
void glutSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint nsides, GLint rings);
void glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void glutSolidTetrahedron();
void glutSolidIcosahedron();
```

### 1.3 주제

**리락쿠마 (Rilakkuma)** — 산엑스(San-X)의 갈색 곰 캐릭터.

---

## 2. 개발 환경

| 항목 | 사양 |
|---|---|
| OS | Windows (둘 다 통일) |
| IDE | Visual Studio 2022 Community |
| 플랫폼 타겟 | **x64** (Win32 ❌) |
| 그래픽스 라이브러리 | freeglut (NuGet 패키지로 설치) |
| 텍스처 로더 | stb_image.h (헤더 온리, thirdparty 폴더에 포함) |
| 버전 관리 | Git + GitHub (private repo) |
| 언어 | C++ (C++14 이상) |

**중요**: 두 팀원의 VS 버전, freeglut 버전, Windows SDK 버전, 플랫폼 타겟을 **반드시 동일하게** 맞춰야 함. 셋업 시 화면 공유하면서 같이 진행.

---

## 3. 일정

| 기간 | 작업 |
|---|---|
| **5/27 ~ 5/29** | 셋업 (repo, 폴더 구조, 씬 그래프 골격, 좌표 단위 합의, 빌드 환경 통일) |
| **~ 6/2 (월)** | 1차 구현 완료 (각자 파트 1차 버전) |
| **6/3 ~ 6/5** | 통합 + 빌드 검증 + 머지 |
| **~ 6/10 (화)** | 최종 구현 (텍스처, 조명, 디테일 보강) |
| **6/11 ~ 6/12** | PPT + 시연 영상 제작 |
| **6/12 (금) 밤** | 제출 (마감 하루 전, 서버 사고 방지) |
| **6/13 (토)** | 마감일 |

---

## 4. 역할 분담

### 4.1 A (너) — 공통 인프라 + 얼굴부

**공통 인프라**
- `main.cpp` — 진입점, GLUT 초기화, 콜백 등록
- `SceneNode` — 씬 그래프 클래스 (transform + renderFunc + children, 재귀 render)
- `Camera` — 카메라 위치, `gluLookAt` 호출, 줌 상태
- `InputHandler` — GLUT 마우스/키보드 콜백 (드래그 회전, a/z 줌)
- `Lighting` — 조명 설정 (`GL_LIGHT0`, ambient/diffuse/specular)
- `TextureLoader` — `stb_image.h` 기반 텍스처 로드 함수
- `MeshUtils` — 공통 메쉬 함수 (반구, 타원체 실린더 등 자체 구현 메쉬)

**얼굴부 (`Head.h` / `Head.cpp`)**
- 머리 (메인 갈색 구, 단색)
- 양 귀 (노란 원 포인트까지 — 귀 안쪽 작은 노란 원)
- 눈 (검은 작은 구, 머리 표면에 박힌 듯하게) — 텍스처 ❌, 별도 메쉬
- 입 주변 흰색 cream 영역 (살짝 돌출된 흰색 타원체) — **여기에 텍스처 매핑**
- 얼굴 텍스처 이미지 제작 (cream 영역에 매핑할 코+입 자수 이미지)

### 4.2 B (팀원) — 몸통부 + 배경 + 텍스처 매핑 코드

**몸통부 (`Body.h` / `Body.cpp`)**
- 몸통 (메인 갈색 구/타원체, 중앙에 흰 배 타원체)
- 양팔 (실린더 또는 타원체 실린더, 손바닥에 노란 타원)
  - **왼팔(캐릭터 기준): 위로 번쩍 든 자세** — 어깨 기준 회전 적용
  - **오른팔(캐릭터 기준): 배 쪽으로 내려진 자세** — 어깨 기준 회전 적용
- 양다리 (실린더 또는 타원체 실린더, 발바닥에 노란 타원, 살짝 벌어진 자세)
- 등 지퍼 (목→꼬리, 작은 토러스/실린더 반복으로 구현)
- 꼬리 (작은 구)

**배경 (`scene/Background.cpp`)**
- 형태 미정 (단색 / 바닥 평면 / 방 안 중 선택)
- B가 자유롭게 결정

**텍스처 매핑 코드**
- A가 만든 얼굴 텍스처 이미지를 cream 영역 메쉬에 매핑하는 코드 작성
- A의 `TextureLoader` 사용
- (선택) 배경에도 텍스처 추가 가능

### 4.3 공통 작업

- 시연 영상 시나리오 협의 및 녹화
- PPT 제작 (분담)
- 통합 빌드 시 머지 충돌 해결

---

## 5. 디자인 결정사항

### 5.1 확정된 사항

| 항목 | 결정 | 비고 |
|---|---|---|
| **포즈** | **한쪽 팔 번쩍 든 인사 포즈** | 첨부 이미지 기준. 캐릭터 기준 왼팔(보는 입장 오른쪽)을 위로 들고, 오른팔은 배 쪽으로 자연스럽게 내림. 다리는 안정감 있게 살짝 벌림 |
| **얼굴 텍스처 방식** | **cream 영역에 코+입 자수 텍스처 매핑** | 머리 구는 단색 갈색. cream 영역(타원체)에만 텍스처. 눈은 별도 작은 검은 구로 입체감 |
| **배경 담당** | **B (팀원)** | 형태는 B가 자유 결정 |
| 몸-팔다리 형태 | **분리** (둥글둥글 타원체) | 도라에몽 방식. 포즈 자유도 ↑ |
| 옆모습 디테일 | **`glScalef`로 구를 눌러 타원체 처리** | 코드 부담 적고 효과 큼 |
| 몸 텍스처 | **얼굴(cream)만 텍스처, 몸은 색상** | 곡면 텍스처 매핑 어려움. 인형 질감은 조명으로 |
| 인형 vs 피규어 질감 | **인형 (specular 낮춤, ambient/diffuse 높임)** | 조명 튜닝만으로 효과 |
| 카메라 초기 시점 | **정면 살짝 위에서** | 얼굴 디테일 보임 |

### 5.2 좌표 단위 규약

#### 5.2.1 부위별 위치

| 부위 | 기준값 |
|---|---|
| 머리 반지름 | **1.0** (모든 크기의 기준) |
| 몸통 반지름 | 약 1.3~1.5 (머리보다 약간 큼) |
| 머리 중심 | (0, 1.5, 0) |
| 몸통 중심 | (0, 0, 0) ← 원점 |
| 왼쪽 어깨 (캐릭터 기준) | (-1.2, 0.5, 0) |
| 오른쪽 어깨 (캐릭터 기준) | (+1.2, 0.5, 0) |
| 왼쪽 다리 시작점 | (-0.6, -1.2, 0) |
| 오른쪽 다리 시작점 | (+0.6, -1.2, 0) |
| 왼쪽 귀 중심 | (-0.7, 2.1, 0) |
| 오른쪽 귀 중심 | (+0.7, 2.1, 0) |
| 꼬리 위치 | (0, 0, -1.4) |
| cream 영역 중심 (얼굴) | (0, 1.3, 0.85) |
| 왼쪽 눈 | (-0.45, 1.7, 0.85) |
| 오른쪽 눈 | (+0.45, 1.7, 0.85) |

> **카메라/캐릭터 좌표계 기준**: +Z = 캐릭터가 보는 방향(앞), +Y = 위, +X = 캐릭터의 오른쪽(보는 입장에서 왼쪽).

#### 5.2.2 포즈별 회전 각도 (인사 포즈)

| 부위 | 회전축 | 각도 | 비고 |
|---|---|---|---|
| **왼팔 (번쩍 든 팔)** | Z축 (어깨 기준) | **+120° ~ +140°** | 위로 들면서 살짝 옆으로. `glRotatef(130, 0, 0, 1)` 정도 |
| **왼팔 추가 회전** | X축 | -10° ~ -20° | 살짝 앞쪽으로 기울임 |
| **오른팔 (배쪽 내린 팔)** | Z축 (어깨 기준) | **-15° ~ -30°** | 거의 자연스럽게 아래, 살짝 안쪽으로 |
| **오른팔 추가 회전** | X축 | +20° ~ +30° | 배 쪽으로 살짝 앞으로 |
| **왼쪽 다리** | Z축 (고관절) | -8° ~ -12° | 약간 벌림 |
| **오른쪽 다리** | Z축 (고관절) | +8° ~ +12° | 약간 벌림 |
| **머리** | (선택) Y축 | 0° ~ -10° | 살짝 옆으로 기울이고 싶으면 |

> **모든 회전은 부품의 로컬 원점에서 적용**. 팔의 로컬 원점은 어깨 위치(부품 위쪽 끝), 다리는 고관절(부품 위쪽 끝)에 둠.

#### 5.2.3 회전 구현 흐름 (예시: 왼팔)

```cpp
SceneNode* leftArm = new SceneNode();
leftArm->setTranslation(-1.2f, 0.5f, 0.0f);   // 어깨 위치로 이동
leftArm->setRotation(130.0f, 0, 0, 1);        // Z축 130도 회전 (위로 번쩍)
// 추가 X축 회전이 필요하면 자식 노드로 감싸거나 행렬 합성
leftArm->setRenderFunction(renderArm);
body->addChild(leftArm);
```

### 5.3 얼굴 텍스처 상세 (A 결정사항)

#### 5.3.1 방식

```
머리 구 (단색 갈색, 텍스처 X)
   │
   ├── cream 영역 (흰색 타원체 메쉬, 약간 돌출)
   │     └── 텍스처 매핑: face_cream.png (코+입 자수)
   │
   ├── 왼쪽 눈 (검은 작은 구, 별도 메쉬, 텍스처 X)
   ├── 오른쪽 눈 (검은 작은 구, 별도 메쉬, 텍스처 X)
   │
   └── 양 귀 (갈색 + 노란 원 포인트, 텍스처 X)
```

#### 5.3.2 텍스처 이미지 사양

- 파일명: `face_cream.png` (또는 `.bmp`)
- 크기: **512x512 px** 권장 (정사각형, 2의 거듭제곱)
- 내용: 흰색 배경 + 중앙에 검은색 Y자형 코+입 자수
  - 코: 작은 둥근 삼각형 형태, 윗부분
  - 입: 코 아래에서 좌우로 갈라지는 Y자 곡선
- 배경: cream 영역 색과 동일한 흰색 (`#F5EEDC` 같은 살짝 따뜻한 흰색)
- 알파 채널: 불필요 (불투명 텍스처)

#### 5.3.3 매핑 방식

- cream 영역은 타원체 (`gluSphere` + `glScalef`로 납작하게)
- `gluQuadricTexture(quad, GL_TRUE)`로 자동 UV 매핑
- 또는 단순 평면 메쉬 (사각형)에 매핑 후 살짝 곡면처럼 배치

### 5.4 컬러 팔레트

| 부위 | 색상 (RGB 0~1) |
|---|---|
| 메인 갈색 (머리, 몸, 팔다리) | `(0.78, 0.62, 0.45)` |
| 귀/얼굴 cream (텍스처 배경) | `(0.96, 0.92, 0.85)` |
| 노란 포인트 (귀 안쪽, 손/발바닥) | `(0.98, 0.85, 0.40)` |
| 흰 배 | `(0.95, 0.93, 0.88)` |
| 눈 (별도 메쉬) | `(0.10, 0.08, 0.08)` |
| 코/입 (텍스처 내) | `(0.20, 0.15, 0.12)` |
| 지퍼 금속 | `(0.70, 0.70, 0.72)` |
| 배경 | B 결정 |

> 최종 색상은 구현 단계에서 튜닝.

---

## 6. 파일 구조

```
RilakkumaProject/
├── .gitignore
├── .gitattributes
├── README.md
├── PROJECT_SPEC.md
├── RilakkumaProject.sln
├── RilakkumaProject.vcxproj
├── RilakkumaProject.vcxproj.filters
│
├── src/
│   ├── main.cpp                      ← 진입점, 콜백 등록
│   │
│   ├── core/                         ← A 담당
│   │   ├── SceneNode.h / .cpp
│   │   ├── Camera.h / .cpp
│   │   ├── InputHandler.h / .cpp
│   │   ├── Lighting.h / .cpp
│   │   ├── TextureLoader.h / .cpp
│   │   └── MeshUtils.h / .cpp
│   │
│   ├── character/
│   │   ├── Rilakkuma.h / .cpp        ← BuildRilakkuma() 루트
│   │   ├── Head.h / .cpp             ← A 담당
│   │   └── Body.h / .cpp             ← B 담당
│   │
│   ├── scene/                        ← B 담당
│   │   ├── Background.h / .cpp
│   │   └── (선택) Prop.h / .cpp
│   │
│   └── thirdparty/
│       └── stb_image.h
│
├── assets/textures/
│   └── face_cream.png                ← A가 제작
│
└── docs/
    ├── coordinate_system.md
    └── meeting_notes.md
```

---

## 7. 핵심 클래스/함수 명세

### 7.1 `SceneNode` (도라에몽 PPT 구조 기반)

```cpp
class SceneNode {
public:
    void setRenderFunction(std::function<void()> func);
    void setTranslation(float x, float y, float z);
    void setRotation(float angleDeg, float ax, float ay, float az);
    void setScale(float sx, float sy, float sz);
    void addChild(SceneNode* child);
    void render();  // 재귀적으로 자신 + 자식 렌더

private:
    std::function<void()> renderFunc;
    float tx, ty, tz;
    float rotAngle, rax, ray, raz;
    float sx, sy, sz;
    std::vector<SceneNode*> children;
};
```

**`render()` 내부 흐름**:
1. `glPushMatrix()`
2. `glTranslatef(tx, ty, tz)`
3. `glRotatef(rotAngle, rax, ray, raz)`
4. `glScalef(sx, sy, sz)`
5. `renderFunc()` 호출 (있으면)
6. 모든 자식의 `render()` 재귀 호출
7. `glPopMatrix()`

### 7.2 `MeshUtils` — 공통 메쉬 함수

```cpp
// 반구 (도라에몽 PPT의 renderHemisphere)
void renderHemisphere(float radius, int slices = 32, int stacks = 16);

// 위아래 다른 타원 단면 실린더 (팔다리용)
void renderTaperedEllipticCylinder(
    float bottomMajor, float bottomMinor,
    float topMajor, float topMinor,
    float height, int slices = 32, int stacks = 16);

// (선택) 캡슐 — 양 끝이 반구로 둥근 실린더
void renderCapsule(float radius, float height, int slices = 32, int stacks = 16);

// 텍스처가 매핑된 타원체 (cream 영역용)
void renderTexturedEllipsoid(float rx, float ry, float rz, GLuint texId,
                              int slices = 32, int stacks = 16);
```

### 7.3 `Camera` + `InputHandler`

- 마우스 드래그 회전: 좌클릭 후 드래그 시 `theta`, `phi` 누적
- `a` 키: 카메라 거리 감소 (or `gluPerspective`의 fov 감소) — 줌인
- `z` 키: 카메라 거리 증가 — 줌아웃
- `gluLookAt(camX, camY, camZ, 0, 0.5f, 0, 0, 1, 0)` (캐릭터 약간 위쪽 바라봄)

### 7.4 `Lighting`

```cpp
void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);   // 스케일된 메쉬 조명 보정 (필수)

    GLfloat ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};   // 인형 질감: 높임
    GLfloat diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};  // 인형 질감: 낮춤
    GLfloat position[] = {5.0f, 10.0f, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}
```

### 7.5 OpenGL 초기 설정 (main.cpp)

```cpp
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);    // 필수 — 스케일된 메쉬 조명 보정
    glEnable(GL_TEXTURE_2D);

    initLighting();
    loadTextures();             // face_cream.png 로드
    rilakkumaRoot = BuildRilakkuma();
    backgroundRoot = BuildBackground();
}
```

### 7.6 빌드 흐름

```
main.cpp
  → init() → BuildRilakkuma() → 루트 SceneNode 리턴
  → glutDisplayFunc(display)
  → display():
       backgroundRoot->render();
       rilakkumaRoot->render();

BuildRilakkuma() {
    SceneNode* root = new SceneNode();
    root->addChild(BuildHead());   // Head.cpp
    root->addChild(BuildBody());   // Body.cpp
    return root;
}

BuildHead() {
    SceneNode* head = new SceneNode();
    head->setTranslation(0, 1.5f, 0);
    head->setRenderFunction(renderHeadSphere);

    // cream 영역 (텍스처 매핑)
    SceneNode* cream = new SceneNode();
    cream->setTranslation(0, -0.2f, 0.85f);
    cream->setScale(0.7f, 0.5f, 0.3f);
    cream->setRenderFunction([](){ renderTexturedEllipsoid(1, 1, 1, faceTexId); });
    head->addChild(cream);

    // 눈
    SceneNode* leftEye = new SceneNode();
    leftEye->setTranslation(-0.45f, 0.2f, 0.85f);
    leftEye->setRenderFunction(renderEye);
    head->addChild(leftEye);

    // ... 오른 눈, 양 귀
    return head;
}

BuildBody() {
    SceneNode* body = new SceneNode();
    body->setRenderFunction(renderBodyMesh);

    // 왼팔 (번쩍 든 팔) — 캐릭터 기준
    SceneNode* leftArm = new SceneNode();
    leftArm->setTranslation(-1.2f, 0.5f, 0.0f);
    leftArm->setRotation(130.0f, 0, 0, 1);
    leftArm->setRenderFunction(renderArm);
    body->addChild(leftArm);

    // 오른팔 (배쪽 내린 팔)
    SceneNode* rightArm = new SceneNode();
    rightArm->setTranslation(1.2f, 0.5f, 0.0f);
    rightArm->setRotation(-25.0f, 0, 0, 1);
    rightArm->setRenderFunction(renderArm);
    body->addChild(rightArm);

    // ... 다리, 흰 배, 등 지퍼, 꼬리
    return body;
}
```

---

## 8. 필수 구현 기능 체크리스트

- [ ] OpenGL 초기 설정 (`GL_DEPTH_TEST`, `GL_LIGHTING`, `GL_NORMALIZE`, `GL_TEXTURE_2D` 등)
- [ ] 씬 그래프 (`SceneNode` 클래스, 재귀 렌더)
- [ ] 공통 메쉬 함수 (반구, 타원체 실린더 등 자체 구현)
- [ ] 텍스처 로더 (`stb_image.h`)
- [ ] 마우스 드래그 3D 회전
- [ ] `a` 키 줌인
- [ ] `z` 키 줌아웃
- [ ] 조명 (ambient + 1개 light, 인형 질감 튜닝)
- [ ] 텍스처 매핑 (cream 영역에 코+입 자수)
- [ ] 리락쿠마 머리 (얼굴 메인 구, 양 귀+노란 포인트, 눈, cream 영역)
- [ ] 리락쿠마 몸 (몸통+흰 배, 양팔+노란 손바닥, 양다리+노란 발바닥, 등 지퍼, 꼬리)
- [ ] **인사 포즈 적용** (왼팔 번쩍, 오른팔 배쪽, 다리 살짝 벌림)
- [ ] 배경 (B 결정)

---

## 9. Git 운용 규칙

### 9.1 브랜치 전략

- `main`: 항상 빌드되는 안정 버전
- `feature/<작업명>`: 작업 브랜치 (예: `feature/scene-node`, `feature/head-eyes`, `feature/body-arms-pose`)
- 작업 끝나면 main에 머지

### 9.2 커밋 단위

작게 쪼개기: "머리 메쉬 추가" / "왼팔 회전 각도 조정" / "cream 영역 텍스처 적용"

### 9.3 파일 분리로 충돌 최소화

- A: `core/*`, `Head.cpp`, `main.cpp` 주로 만짐
- B: `Body.cpp`, `Background.cpp`, 텍스처 매핑 코드 주로 만짐
- 공통 파일 (`SceneNode.h`, `MeshUtils.h`, `Rilakkuma.cpp`)을 양쪽이 건드릴 때만 조심

### 9.4 `.gitignore`

```
# Build output
build/ bin/ obj/ Debug/ Release/ x64/ ipch/

# Visual Studio
.vs/ *.user *.suo *.opensdf *.sdf *.aps *.opendb *.db

# Misc
*.log Thumbs.db
```

### 9.5 `.gitattributes`

```
* text=auto
*.png binary
*.bmp binary
*.jpg binary
```

---

## 10. 제출물 체크리스트

- [ ] **소스 코드 zip** (`학번.zip`) — Visual Studio 프로젝트 파일 포함, .cpp/.h 전부, 텍스처 포함
- [ ] **PPT** (`학번.ppt`) — 첫 장에 팀원 학번+이름, 함수 설명, 실행 사례
- [ ] **시연 동영상** — 곰캠/Camtasia/OBS 등으로 녹화

### 10.1 PPT 구성

1. 표지 (팀원 학번/이름)
2. 목차
3. 개발 목표 / 주제 소개 (리락쿠마 인사 포즈 컨셉)
4. 개발 환경 (Windows, VS2022, freeglut)
5. 전체 구조 (씬 그래프 다이어그램)
6. 모델링 구현 — 얼굴부 (함수별 설명)
7. 모델링 구현 — 몸통부 + 포즈 (함수별 설명, 회전 각도 어필)
8. 자체 구현 메쉬 함수 (반구, 타원체 실린더 등)
9. 텍스처 매핑 (cream 영역 + face_cream.png)
10. 조명 / 카메라 / 입력 처리
11. 실행 화면 (정면 / 회전 / 줌인 / 줌아웃)
12. 데모 영상 링크
13. 결론, 느낀점

### 10.2 시연 영상 시나리오

1. 정면 초기 화면 (인사 포즈 어필)
2. 마우스 드래그로 360° 회전 (등 지퍼 보여주기)
3. `a` 키 줌인 → 얼굴 클로즈업 (cream 영역 텍스처 디테일)
4. `z` 키 줌아웃 → 전체 모습
5. 길이: 30초~1분

---

## 11. 셋업 단계 (우선순위 순)

1. **GitHub private repo 생성**, 두 팀원 collaborator로 추가
2. **Visual Studio 2022 + freeglut(NuGet) 설치**, 양쪽 환경 통일
3. **빈 VS 프로젝트 생성**, x64 플랫폼 설정, freeglut 링크 확인
4. **파일 구조(섹션 6) 그대로 빈 파일 생성** 후 첫 커밋
5. **`SceneNode.h/cpp` 작성** → 도라에몽 스타일
6. **`MeshUtils.h/cpp` 작성** → 반구, 타원체 실린더
7. **`Camera`, `InputHandler`, `Lighting` 작성** → 드래그/줌/조명 동작 확인
8. **갈색 구 하나 띄워보기** → 인프라 전체 동작 검증
9. 이후 Head/Body 각자 작업 시작

---

## 12. 위험 요소 & 대응

| 위험 | 대응 |
|---|---|
| freeglut 링크 실패 | NuGet 패키지 설치 화면 공유로 함께 진행 |
| 좌표 단위 불일치로 통합 후 비례 깨짐 | 섹션 5.2 좌표 표를 기준으로 처음부터 통일 |
| 머지 충돌 | 파일 분리 + 작은 커밋 + 자주 push |
| 텍스처 매핑 시간 부족 | cream 영역 한 곳만 우선 적용 |
| cream 영역 텍스처 왜곡 | `gluQuadricTexture(quad, GL_TRUE)` 사용. 안 되면 평면 사각형으로 대체 |
| 팔 회전이 어색하게 보임 | 어깨 위치를 부품 로컬 원점으로 잡았는지 확인. 자식 노드 회전 합성 활용 |
| 마감 직전 빌드 깨짐 | 6/12 밤 제출 (6/13은 예비일) |

---

## 13. 미확정 사항 (친구가 결정)

- [ ] **배경 형태** — 단색 / 바닥 평면 / 방 안 등
- [ ] **오브제 추가 여부 + 종류** (시간 남으면, 본체 완성 후)
- [ ] **최종 빌드 + 영상 녹화 담당 PC**
- [ ] **VS / freeglut 정확한 버전** (양쪽 통일)

## 14. 확정된 사항 (요약)

- ✅ 주제: 리락쿠마
- ✅ 팀원: A(공통 인프라 + 얼굴부), B(몸통부 + 배경 + 텍스처 매핑 코드)
- ✅ 포즈: 한쪽 팔 번쩍 든 인사 포즈 (이미지 기준)
- ✅ 얼굴 텍스처: cream 영역에 코+입 자수 텍스처 매핑 (A가 이미지 제작)
- ✅ 좌표 단위: 머리 반지름 = 1.0
- ✅ 환경: Windows + VS2022 + freeglut + x64
- ✅ Git: private repo, feature 브랜치
- ✅ 일정: 6/12 밤 제출 (6/13 예비)
