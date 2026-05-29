# Rilakkuma — 3D Virtual Human Modeling & Visualization

컴퓨터 그래픽스 기말 프로젝트. OpenGL(freeglut) 기반으로 산엑스 캐릭터
**리락쿠마**를 인사 포즈로 모델링하고, 마우스 회전 / 줌 / 텍스처 매핑으로
시각화한다.

## 빠른 시작 (Windows + Visual Studio 2022)

1. `git clone` 후 작업 브랜치 체크아웃.
2. `RilakkumaProject.sln` 열기 → 구성 **Debug / x64**.
3. 솔루션 우클릭 → **NuGet 패키지 복원** (freeglut = `nupengl.core`).
4. **F5** 실행.
   - 마우스 좌클릭 드래그: 3D 회전
   - `a`: zoom-in / `z`: zoom-out / `ESC`: 종료

> 상세 환경 셋업(아무것도 안 깔린 PC 기준)은 **[docs/WINDOWS_SETUP.md](docs/WINDOWS_SETUP.md)**.

## 구조

```
src/
  main.cpp                  진입점, GLUT 콜백
  core/                     [A] 공통 인프라
    SceneNode  씬 그래프(재귀 렌더)
    Camera     궤도 카메라(드래그 회전/줌)
    InputHandler  마우스/키보드
    Lighting   조명(인형 질감)
    TextureLoader  stb_image 텍스처 로드
    MeshUtils  자체 구현 메쉬(반구/타원체/실린더/캡슐)
  character/
    Rilakkuma  루트 빌더 + 컬러 팔레트
    Head       [A] 얼굴부 (머리/귀/눈/cream 텍스처) — 완성
    Body       [B] 몸통부 — 플레이스홀더 + 가이드
  scene/
    Background [B] 배경 — 플레이스홀더
  thirdparty/stb_image.h
assets/textures/face_cream.png   얼굴 코+입 자수 텍스처
tools/make_face_texture.py       텍스처 재생성 스크립트
docs/                            셋업/좌표/가이드/회의록
```

## 담당

- **A**: 공통 인프라 + 얼굴부 + 얼굴 텍스처 (이 커밋에서 완성).
- **B**: 몸통부 + 배경 + 텍스처 매핑 확장 — **[docs/B_GUIDE.md](docs/B_GUIDE.md)** 참고.

## 과제 요구사항 충족

- [x] 3D 객체만으로 모델링, 원점 기준 + `glTranslatef`/`glRotatef` 배치
- [x] 마우스 드래그 3D 회전
- [x] `a` 줌인 / `z` 줌아웃
- [x] 텍스처 매핑 (cream 영역 코+입 자수)
- [x] 조명 (GL_LIGHT0, 인형 질감 튜닝)
- [x] 자체 구현 메쉬 (메쉬 파일 로드 ❌)
- [ ] 몸통/팔다리/포즈/배경 (B)
