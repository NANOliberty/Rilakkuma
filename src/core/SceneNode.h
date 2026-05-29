#pragma once
//==============================================================================
// SceneNode.h  -  씬 그래프 노드 (도라에몽 PPT 구조 기반)
//
//  하나의 노드는 [지역 변환(translate/rotate/scale)] + [렌더 함수] + [자식들]을
//  가진다. render() 는 자신의 변환을 행렬 스택에 쌓고 -> 자기 메쉬를 그리고 ->
//  자식들을 재귀적으로 그린 뒤 행렬을 복원한다.
//
//  과제 요구사항("모든 요소는 원점 기준으로 만들고 glTranslatef/glRotatef 로
//  부착")을 그대로 만족시키는 구조다. 각 renderFunc 은 원점 기준으로만 메쉬를
//  그리고, 위치/포즈는 SceneNode 의 변환으로 결정한다.
//==============================================================================
#include <functional>
#include <vector>

class SceneNode {
public:
    SceneNode();
    ~SceneNode();

    // 이 노드가 그릴 메쉬. 원점 기준으로만 그리면 된다(위치는 변환이 담당).
    void setRenderFunction(std::function<void()> func);

    // 지역 변환 설정. render() 시 translate -> rotate -> scale 순으로 적용된다.
    void setTranslation(float x, float y, float z);
    void setRotation(float angleDeg, float ax, float ay, float az);
    void setScale(float sx, float sy, float sz);

    // 자식 노드 추가. 소유권을 가지며 소멸 시 delete 한다.
    void addChild(SceneNode* child);

    // 자신 + 모든 자식을 재귀적으로 렌더.
    void render();

private:
    std::function<void()> renderFunc;

    float tx, ty, tz;                  // translation
    float rotAngle, rax, ray, raz;     // rotation (각도 + 축)
    float sx, sy, sz;                  // scale

    std::vector<SceneNode*> children;
};
