# 회의록 / 결정사항 로그

## 2026-05-29 — 셋업 & A 파트 1차 구현
- 역할: A = 공통 인프라 + 얼굴부 / B = 몸통부 + 배경 + 텍스처 매핑 코드.
- 이번 작업 범위 결정:
  - **A 파트는 완성** (core 인프라 + Head + 얼굴 텍스처 이미지 제작).
  - **B 파트는 파일 골격 + 가이드만** (Body/Background 플레이스홀더 + docs/B_GUIDE.md).
  - **VS 프로젝트 파일 생성**(.sln/.vcxproj/.filters/packages.config) — nupengl.core(NuGet)로 freeglut 링크.
- 완료:
  - SceneNode / Camera / InputHandler / Lighting / TextureLoader / MeshUtils.
  - Head: 머리 구, 양 귀(노란 포인트), 눈, cream 영역 곡면 텍스처 패치.
  - `assets/textures/face_cream.png` 생성(코+입 자수, 512x512). 재생성 스크립트: `tools/make_face_texture.py`.
  - Linux(freeglut)에서 컴파일·링크 검증 완료 (Windows 와 동일 API).
- TODO:
  - [ ] (B) Body: 팔/다리/손발바닥/지퍼/꼬리 + 인사 포즈 회전.
  - [ ] (B) Background 형태 결정.
  - [ ] 윈도우에서 실제 빌드/실행 1회 검증 (docs/WINDOWS_SETUP.md).
  - [ ] VS/freeglut 버전 양쪽 통일 확인.
  - [ ] PPT 첫 장 학번/이름.

## 미확정 (PROJECT_SPEC 13)
- [ ] 배경 형태(단색/바닥/방 안) — B 결정.
- [ ] 오브제 추가 여부.
- [ ] 최종 빌드·녹화 담당 PC.
- [ ] VS/freeglut 정확한 버전(양쪽 통일).
