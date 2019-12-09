#ifndef BOXSCENE_H
#define BOXSCENE_H
#include <QtOpenGL>
#include "camera.h"
#include "solvers.h"
#include "structs.h"

class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

enum CameraType
{
    FreeCamera,
    OXY,
    OXZ,
    OZY
};
enum VertexBufferType
{
    BOXGEOM=0,
    BOXOUTLINE=1,
    SCREENQUAD=2,
    SKYBOXGEOM=3,
    CONTAINER=4,
    QUAD=5,
    BUFFERS=6
};
enum ShaderProgramType
{
    SKYBOX=0,
    SCREEN=1,
    COLOR=2,
    OUTLINE=3,
    BILLBOARD=4,
    PROGRAMS=5
};
struct BoxRenderInfo
{
    QMatrix4x4 model;
    QVector3D color;
    bool selected;
};

class BoxScene : public QOpenGLWidget
{
    Q_OBJECT

    QMatrix4x4 xModel1;
    QMatrix4x4 yModel1;
    QMatrix4x4 zModel1;
    QMatrix4x4 xModel2;
    QMatrix4x4 yModel2;
    QMatrix4x4 zModel2;
#define NEW_RENDER
#ifdef NEW_RENDER
    QMatrix4x4 envelope;
    QVector<BoxRenderInfo> boxes;
    QOpenGLShaderProgram* programs[PROGRAMS] ;
    QOpenGLBuffer* vbo[BUFFERS];
    QOpenGLVertexArrayObject* vao[BUFFERS];
#endif

    QOpenGLTexture * fontTexture;

   /* QOpenGLBuffer* vboContainer;
    QOpenGLVertexArrayObject* vaoContainer;*/
    QMatrix4x4 containerModel;
    int containerCount;


   /* QOpenGLBuffer* vboSkybox;
    QOpenGLVertexArrayObject* vaoSkybox;
    QOpenGLBuffer* vboScreen;
    QOpenGLVertexArrayObject* vaoScreen;*/

    bool initialized;

 /*   QOpenGLBuffer* vboBoxes;
    QOpenGLVertexArrayObject* vaoBoxes;*/

/*
    QOpenGLShaderProgram* boxshader;
    QOpenGLShaderProgram* containershader;
    QOpenGLShaderProgram* skyboxshader;
    QOpenGLShaderProgram* screenshader;*/
    int boxCount;
    QPoint lastPos;
    bool mouseCaptured;
    SphericalCamera perspCamera;
    OrthoCamera oXY;
    OrthoCamera oXZ;
    OrthoCamera oZY;
    float w;
    float h;
    CameraType cameraType;
signals:
    void updateDistance(float distanceNormalized);
public slots:
    void setDistance(float distanceNormalized);
protected:
   void initializeGL();
   void resizeGL(int w, int h);
   void paintGL();
   void mousePressEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void mouseMoveEvent(QMouseEvent *event);
   void leaveEvent(QEvent *event);
   void wheelEvent(QWheelEvent *event);
public:
   void select(const QVector<int>& indexes);
   void setCameraType(CameraType type);
   BoxScene(QWidget* parent = 0);
   void init(QVector<BoxArrayStruct>& boxArray, Box bounds);
   ~BoxScene();
};

#endif // BOXSCENE_H
