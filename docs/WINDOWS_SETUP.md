# Windows 개발 환경 셋업 가이드

> 아직 윈도우 데스크탑에 아무것도 안 깔려 있어도 이 문서만 순서대로 따라오면
> `RilakkumaProject.sln` 을 열어 **F5 한 번으로 리락쿠마가 뜨는** 상태까지 갑니다.
> 두 팀원이 **같은 버전**으로 맞추는 게 핵심입니다(섹션 0).

---

## 0. 두 사람이 통일해야 하는 것 (먼저 합의!)

| 항목 | 값 |
|---|---|
| Visual Studio | **2022 Community** (17.x) |
| 플랫폼 타겟 | **x64** (Win32 ❌) |
| 플랫폼 도구 집합 | **v143** |
| Windows SDK | **10.0 (latest installed)** |
| OpenGL 라이브러리 | **nupengl.core** (NuGet, freeglut+GLEW 포함) |
| C++ 표준 | **C++14** |

이 프로젝트(.vcxproj)는 위 값으로 이미 설정돼 있습니다.

---

## 1. Visual Studio 2022 설치

1. https://visualstudio.microsoft.com/ko/vs/community/ 에서 **Community 2022** 다운로드.
2. 설치 관리자에서 워크로드 **"C++를 사용한 데스크톱 개발"** 체크.
   - 우측 "설치 세부 정보"에 아래가 포함되는지 확인:
     - MSVC v143 빌드 도구 (x64/x86)
     - Windows 11 SDK (또는 10 SDK 최신)
     - C++ CMake 도구(선택)
3. 설치 완료 후 재부팅.

---

## 2. 저장소 받기 (Git)

```powershell
# Git 미설치 시: https://git-scm.com/download/win
git clone <레포 URL>
cd Rilakkuma
git checkout claude/brave-turing-6ndyG   # 작업 브랜치
```

> GitHub private repo 에 두 사람 모두 collaborator 로 추가돼 있어야 합니다.

---

## 3. 솔루션 열기 + NuGet 패키지 복원 (freeglut)

1. `RilakkumaProject.sln` 더블클릭 → Visual Studio 가 열림.
2. 상단 구성 콤보를 **Debug / x64** 로 설정 (Win32 아님!).
3. **NuGet 복원**: 솔루션 탐색기에서 *솔루션* 우클릭 → **NuGet 패키지 복원**.
   - 또는 메뉴 *도구 → NuGet 패키지 관리자 → 패키지 관리자 콘솔* 에서:
     ```
     Update-Package -reinstall
     ```
   - `packages.config` 에 적힌 `nupengl.core`, `nupengl.core.redist` 가
     `packages\` 폴더로 받아집니다. (이 폴더는 .gitignore 처리됨 → 각자 복원)
4. nupengl.core 가 freeglut/GLEW 의 헤더·lib·dll 을 **자동으로 링크하고
   실행 시 DLL 도 출력 폴더로 복사**해 줍니다. 별도 링커 설정 불필요.

> 만약 빌드 시 "빠진 NuGet 패키지" 오류가 나면 3번 복원이 안 된 것입니다.
> *도구 → 옵션 → NuGet 패키지 관리자 → 일반 → "빌드하는 동안 누락된 패키지
> 자동 복원 허용"* 을 켜고 다시 빌드하세요.

---

## 4. 빌드 & 실행

1. **Ctrl+Shift+B** (빌드) → 오류 0 확인.
2. **F5** (디버그 실행) 또는 **Ctrl+F5**.
3. 창이 뜨고 리락쿠마(머리+얼굴 텍스처+몸통)가 보이면 성공.
   - 마우스 좌클릭 드래그 → 회전
   - `a` 줌인 / `z` 줌아웃 / `ESC` 종료

### 텍스처(face_cream.png) 경로 주의
- 코드는 `assets/textures/face_cream.png` 를 **작업 디렉터리 기준 상대경로**
  로 찾습니다. 후보로 `./`, `../`, `../../` 도 시도합니다.
- VS 기본 작업 디렉터리는 `$(ProjectDir)`(=레포 루트)라 그대로 찾아집니다.
- 못 찾으면 콘솔에 경고가 뜨고 cream 영역이 **단색**으로 표시됩니다(크래시 X).
- 굳이 바꾸려면 *프로젝트 속성 → 디버깅 → 작업 디렉터리* 를 `$(ProjectDir)` 로.

---

## 5. 자주 나는 문제

| 증상 | 원인/해결 |
|---|---|
| `freeglut.h 를 찾을 수 없음` | NuGet 복원 안 됨 → 섹션 3 다시. 구성이 x64 인지 확인 |
| `LNK2019 ... glut*` 링크 오류 | nupengl 미복원 또는 Win32 로 빌드 중 → x64 로 |
| 실행 시 `freeglut.dll 없음` | nupengl.core.redist 미복원 → 복원 후 재빌드 |
| 화면 새까맣고 캐릭터 안 보임 | 카메라 거리 문제 드묾 → `z` 로 줌아웃 |
| cream 영역이 흰 단색 | 텍스처 경로 못 찾음 → 콘솔 로그 확인, 섹션 4 |
| 한글 주석 깨짐 | 파일 인코딩 UTF-8 유지(.gitattributes 적용됨) |

---

## 6. (대안) NuGet 대신 수동 freeglut

nupengl 이 정 안 되면 freeglut 를 수동 링크할 수도 있습니다(권장 X):
1. https://www.transmissionzero.co.uk/software/freeglut-devel/ 에서 MSVC 패키지.
2. include / lib / bin 을 적절히 배치하고 프로젝트 속성에서
   포함 디렉터리·라이브러리 디렉터리·추가 종속성(`freeglut.lib`)·DLL 복사 설정.
3. 헤더는 동일하게 `#include <GL/freeglut.h>` 사용.

> 두 사람이 같은 방식(가급적 nupengl)으로 맞추는 걸 강력 추천합니다.
