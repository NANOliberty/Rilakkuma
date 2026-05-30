#include "SceneNode.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>

SceneNode::SceneNode()
    : renderFunc(nullptr),
      tx(0.0f), ty(0.0f), tz(0.0f),
      rotAngle(0.0f), rax(0.0f), ray(1.0f), raz(0.0f),
      sx(1.0f), sy(1.0f), sz(1.0f) {}

SceneNode::~SceneNode() {
    for (SceneNode* child : children) {
        delete child;
    }
    children.clear();
}

void SceneNode::setRenderFunction(std::function<void()> func) {
    renderFunc = std::move(func);
}

void SceneNode::setTranslation(float x, float y, float z) {
    tx = x; ty = y; tz = z;
}

void SceneNode::setRotation(float angleDeg, float ax, float ay, float az) {
    rotAngle = angleDeg; rax = ax; ray = ay; raz = az;
}

void SceneNode::setScale(float x, float y, float z) {
    sx = x; sy = y; sz = z;
}

void SceneNode::addChild(SceneNode* child) {
    if (child) children.push_back(child);
}

void SceneNode::render() {
    glPushMatrix();

    // 1) 위치  2) 회전  3) 스케일  순서로 지역 변환 적용
    glTranslatef(tx, ty, tz);
    glRotatef(rotAngle, rax, ray, raz);
    glScalef(sx, sy, sz);

    // 4) 이 노드의 메쉬 렌더
    if (renderFunc) renderFunc();

    // 5) 자식들 재귀 렌더 (부모 변환이 누적된 상태)
    for (SceneNode* child : children) {
        child->render();
    }

    glPopMatrix();
}
