#include "Background.h"
#include "../core/SceneNode.h"
#include "../core/TextureLoader.h"
#include "../core/Lighting.h"
#include "../core/MeshUtils.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>

static GLuint g_floorTexId = 0;
static GLuint g_artTexLarge = 0;
static GLuint g_artTexMedium = 0;
static GLuint g_artTexSmall = 0;
static GLuint g_artTexSofa = 0;

static const float kFloorY = -2.4f;
static const float kCeilY = 6.0f;
static const float kWallBack = -8.0f;
static const float kWallFront = 4.0f;
static const float kWallL = -7.0f;
static const float kWallR = 7.0f;

static void colorSofa() { glColor3f(0.98f, 0.82f, 0.28f); }  
static void colorWall() { glColor3f(0.78f, 0.91f, 0.97f); }  
static void colorFrame() { glColor3f(0.90f, 0.82f, 0.68f); } 
static void colorArt() { glColor3f(0.95f, 0.95f, 0.88f); }
static void colorWindow() { glColor3f(0.96f, 0.98f, 1.00f); }
static void colorSky() { glColor3f(0.72f, 0.88f, 0.98f); }
static void colorWood() { glColor3f(0.76f, 0.58f, 0.38f); }
static void colorPot() { glColor3f(0.92f, 0.95f, 0.96f); }  
static void colorSoil() { glColor3f(0.32f, 0.22f, 0.18f); } 
static void colorLeaf() { glColor3f(0.24f, 0.58f, 0.32f); }
static void colorCarpet() { glColor3f(0.98f, 0.94f, 0.72f); }

static void drawFrameProp(float zCenter, float yCenter, float w, float h, GLuint texId) {
    glPushMatrix();
    glTranslatef(kWallL + 0.03f, yCenter, zCenter);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

    float border = 0.05f;
    colorFrame();
    glPushMatrix(); { glTranslatef(0.0f, h / 2.0f + border / 2.0f, 0.0f); glScalef(w + border * 2.0f, border, 0.04f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(0.0f, -h / 2.0f - border / 2.0f, 0.0f); glScalef(w + border * 2.0f, border, 0.04f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(-w / 2.0f - border / 2.0f, 0.0f, 0.0f); glScalef(border, h, 0.04f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(w / 2.0f + border / 2.0f, 0.0f, 0.0f); glScalef(border, h, 0.04f); glutSolidCube(1.0f); } glPopMatrix();

    bool textured = (texId != 0);
    if (textured) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    else {
        colorArt();
    }

    // ※ 이 액자는 -90° Y회전이 들어가 그림 면이 바깥(−x 월드)을 향한다. 백페이스
    //   컬링을 켜면 안에서 뒷면이 되어 잘리므로, 정점 감김을 뒤집어 그림 면이
    //   방 안쪽을 향하게 한다(텍스처 좌표는 각 정점 그대로라 그림은 동일).
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w / 2.0f, h / 2.0f, 0.01f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w / 2.0f, h / 2.0f, 0.01f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w / 2.0f, -h / 2.0f, 0.01f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w / 2.0f, -h / 2.0f, 0.01f);
    glEnd();

    if (textured) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
}

static void drawBackWallFrameProp(float xCenter, float yCenter, float w, float h, GLuint texId) {
    glPushMatrix();
    glTranslatef(xCenter, yCenter, kWallBack + 0.03f);

    float border = 0.05f;
    colorFrame();
    glPushMatrix(); { glTranslatef(0.0f, h / 2.0f + border / 2.0f, 0.0f); glScalef(w + border * 2.0f, border, 0.04f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(0.0f, -h / 2.0f - border / 2.0f, 0.0f); glScalef(w + border * 2.0f, border, 0.04f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(-w / 2.0f - border / 2.0f, 0.0f, 0.0f); glScalef(border, h, 0.04f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(w / 2.0f + border / 2.0f, 0.0f, 0.0f); glScalef(border, h, 0.04f); glutSolidCube(1.0f); } glPopMatrix();

    bool textured = (texId != 0);
    if (textured) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    else {
        colorArt();
    }

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w / 2.0f, -h / 2.0f, 0.01f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(w / 2.0f, -h / 2.0f, 0.01f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(w / 2.0f, h / 2.0f, 0.01f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-w / 2.0f, h / 2.0f, 0.01f);
    glEnd();

    if (textured) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
}

static void renderFloor() {
    Lighting::applyPlushMaterial();
    bool textured = (g_floorTexId != 0);
    if (textured) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_floorTexId);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    else {
        colorWood();
    }
    // 정점 감김이 윗면(+y)이 앞면이 되도록(백페이스 컬링 시 위에서 봐도 안 잘림).
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.857f); glVertex3f(kWallL, kFloorY, kWallFront);
    glTexCoord2f(1.0f, 0.857f); glVertex3f(kWallR, kFloorY, kWallFront);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f(kWallR, kFloorY, kWallBack);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(kWallL, kFloorY, kWallBack);
    glEnd();
    if (textured) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}


static void renderCarpet() {
    Lighting::applyPlushMaterial();
    colorCarpet();

    glPushMatrix();
    glTranslatef(0.0f, kFloorY + 0.02f, -0.7f);
    glScalef(9.6f, 0.04f, 6.6f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

static void renderBackWall() {
    Lighting::applyPlushMaterial();
    colorWall();
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(kWallL, kFloorY, kWallBack);
    glVertex3f(kWallR, kFloorY, kWallBack);
    glVertex3f(kWallR, kCeilY, kWallBack);
    glVertex3f(kWallL, kCeilY, kWallBack);
    glEnd();
    drawBackWallFrameProp(0.0f, 3.8f, 4.80f, 2.70f, g_artTexSofa);
}

static void renderLeftWall() {
    Lighting::applyPlushMaterial();
    colorWall();
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(kWallL, kFloorY, kWallFront);
    glVertex3f(kWallL, kFloorY, kWallBack);
    glVertex3f(kWallL, kCeilY, kWallBack);
    glVertex3f(kWallL, kCeilY, kWallFront);
    glEnd();
    drawFrameProp(-2.2f, 3.5f, 3.20f, 1.80f, g_artTexLarge);
    drawFrameProp(-4.6f, 1.6f, 2.13f, 1.20f, g_artTexMedium);
    drawFrameProp(0.1f, 1.9f, 1.42f, 0.80f, g_artTexSmall);
}

static void renderRightWall() {
    Lighting::applyPlushMaterial();
    colorWall();
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(kWallR, kFloorY, kWallBack);
    glVertex3f(kWallR, kFloorY, kWallFront);
    glVertex3f(kWallR, kCeilY, kWallFront);
    glVertex3f(kWallR, kCeilY, kWallBack);
    glEnd();

    glPushMatrix();
    glTranslatef(kWallR - 0.05f, 1.4f, -2.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // 감김을 방 안쪽(-x 월드)이 앞면이 되도록 뒤집음(컬링 시 안 잘리게).
    colorSky();
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-4.0f, 3.8f, -0.01f);
    glVertex3f(4.0f, 3.8f, -0.01f);
    glVertex3f(4.0f, -3.8f, -0.01f);
    glVertex3f(-4.0f, -3.8f, -0.01f);
    glEnd();

    colorWindow();
    glPushMatrix(); { glTranslatef(0.0f, 3.80f, 0.0f); glScalef(8.10f, 0.15f, 0.10f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(0.0f, -3.80f, 0.0f); glScalef(8.10f, 0.15f, 0.10f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(-4.00f, 0.0f, 0.0f); glScalef(0.15f, 7.60f, 0.10f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(4.00f, 0.0f, 0.0f); glScalef(0.15f, 7.60f, 0.10f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(0.0f, 0.0f, 0.0f); glScalef(0.12f, 7.60f, 0.08f); glutSolidCube(1.0f); } glPopMatrix();
    glPopMatrix();
}

static void renderSofa() {
    Lighting::applyPlushMaterial();
    colorSofa();
    glPushMatrix();
    glTranslatef(0.0f, kFloorY + 0.04f, -6.11f);
    glScalef(2.0f, 2.0f, 2.0f);

    glPushMatrix(); { glTranslatef(0.0f, 0.50f, 0.0f); glScalef(3.20f, 0.65f, 1.80f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(0.0f, 1.30f, -0.72f); glScalef(3.20f, 1.30f, 0.45f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(-1.50f, 0.85f, 0.0f); glScalef(0.42f, 1.00f, 1.80f); glutSolidCube(1.0f); } glPopMatrix();
    glPushMatrix(); { glTranslatef(1.50f, 0.85f, 0.0f); glScalef(0.42f, 1.00f, 1.80f); glutSolidCube(1.0f); } glPopMatrix();

    colorFrame();
    const float lx[4] = { -1.20f, 1.20f, -1.20f, 1.20f };
    const float lz[4] = { -0.65f, -0.65f, 0.65f, 0.65f };
    for (int i = 0; i < 4; ++i) {
        glPushMatrix();
        glTranslatef(lx[i], 0.12f, lz[i]);
        glScalef(0.22f, 0.28f, 0.22f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    glPopMatrix();
}

static void renderPlantPot() {
    Lighting::applyPlushMaterial();
    glPushMatrix();
    glTranslatef(4.8f, kFloorY, -5.8f);
    glScalef(1.7f, 1.7f, 1.7f);

    glPushMatrix(); colorPot(); glTranslatef(0.0f, 0.7f, 0.0f); glRotatef(90.0f, 1.0f, 0.0f, 0.0f); glutSolidCylinder(0.38f, 0.7f, 20, 20); glPopMatrix();
    glPushMatrix(); colorSoil(); glTranslatef(0.0f, 0.69f, 0.0f); MeshUtils::renderEllipsoid(0.36f, 0.02f, 0.36f, 16, 16); glPopMatrix();
    glPushMatrix(); colorLeaf(); glTranslatef(0.0f, 1.45f, 0.0f); glRotatef(-3.0f, 0.0f, 0.0f, 1.0f); MeshUtils::renderCapsule(0.035f, 1.5f); glPopMatrix();

    colorLeaf();
    glPushMatrix(); glTranslatef(0.0f, 1.5f, 0.0f);
    glPushMatrix(); glRotatef(50.0f, 0.0f, 0.0f, 1.0f); glRotatef(15.0f, 1.0f, 0.0f, 0.0f); glTranslatef(-0.22f, 0.0f, 0.0f); MeshUtils::renderEllipsoid(0.35f, 0.03f, 0.18f, 12, 12); glPopMatrix();
    glPushMatrix(); glRotatef(-50.0f, 0.0f, 0.0f, 1.0f); glRotatef(15.0f, 1.0f, 0.0f, 0.0f); glTranslatef(0.22f, 0.0f, 0.0f); MeshUtils::renderEllipsoid(0.35f, 0.03f, 0.18f, 12, 12); glPopMatrix();
    glPopMatrix();

    glPushMatrix(); glTranslatef(0.0f, 1.85f, 0.0f);
    glPushMatrix(); glRotatef(35.0f, 1.0f, 0.0f, 0.0f); glTranslatef(0.0f, 0.0f, 0.22f); MeshUtils::renderEllipsoid(0.16f, 0.03f, 0.32f, 12, 12); glPopMatrix();
    glPushMatrix(); glRotatef(-40.0f, 1.0f, 0.0f, 0.0f); glTranslatef(0.0f, 0.0f, -0.22f); MeshUtils::renderEllipsoid(0.16f, 0.03f, 0.32f, 12, 12); glPopMatrix();
    glPopMatrix();

    glPushMatrix(); glTranslatef(0.0f, 2.15f, 0.0f);
    glPushMatrix(); glRotatef(25.0f, 0.0f, 0.0f, 1.0f); glTranslatef(-0.1f, 0.1f, 0.0f); MeshUtils::renderEllipsoid(0.20f, 0.03f, 0.12f, 12, 12); glPopMatrix();
    glPushMatrix(); glRotatef(-25.0f, 0.0f, 0.0f, 1.0f); glTranslatef(0.1f, 0.1f, 0.0f); MeshUtils::renderEllipsoid(0.20f, 0.03f, 0.12f, 12, 12); glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}

SceneNode* BuildBackground() {
    g_floorTexId = TextureLoader::load2D("assets/textures/floor_wood.png");
    g_artTexLarge = TextureLoader::load2D("assets/textures/art_large.png");
    g_artTexMedium = TextureLoader::load2D("assets/textures/art_medium.png");
    g_artTexSmall = TextureLoader::load2D("assets/textures/art_small.png");
    g_artTexSofa = TextureLoader::load2D("assets/textures/art_sofa.png");

    SceneNode* bg = new SceneNode();

    SceneNode* floor = new SceneNode(); floor->setRenderFunction(renderFloor); bg->addChild(floor);
    SceneNode* carpet = new SceneNode(); carpet->setRenderFunction(renderCarpet); bg->addChild(carpet);
    SceneNode* backWall = new SceneNode(); backWall->setRenderFunction(renderBackWall); bg->addChild(backWall);
    SceneNode* leftWall = new SceneNode(); leftWall->setRenderFunction(renderLeftWall); bg->addChild(leftWall);
    SceneNode* rightWall = new SceneNode(); rightWall->setRenderFunction(renderRightWall); bg->addChild(rightWall);
    SceneNode* sofa = new SceneNode(); sofa->setRenderFunction(renderSofa); bg->addChild(sofa);
    SceneNode* plantPot = new SceneNode(); plantPot->setRenderFunction(renderPlantPot); bg->addChild(plantPot);

    return bg;
}