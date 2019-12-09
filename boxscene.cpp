
#include "boxscene.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QDebug>

void BoxScene::initializeGL()
{
    initialized=false;

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    fontTexture=new QOpenGLTexture(QImage(QString("./Resources/tex21.png")).mirrored());

    QOpenGLShader skyboxshaderVertex(QOpenGLShader::Vertex);
    QOpenGLShader skyboxshaderFragment(QOpenGLShader::Fragment);
    skyboxshaderVertex.compileSourceFile("./Shaders/skybox.vert");
    skyboxshaderFragment.compileSourceFile("./Shaders/skybox.frag");
    programs[ShaderProgramType::SKYBOX]=new QOpenGLShaderProgram();
    programs[ShaderProgramType::SKYBOX]->addShader(&skyboxshaderVertex);
    programs[ShaderProgramType::SKYBOX]->addShader(&skyboxshaderFragment);
    programs[ShaderProgramType::SKYBOX]->bindAttributeLocation("vertex", 0);
    programs[ShaderProgramType::SKYBOX]->link();

    QOpenGLShader screenshaderVertex(QOpenGLShader::Vertex);
    QOpenGLShader screenshaderFragment(QOpenGLShader::Fragment);
    screenshaderVertex.compileSourceFile("./Shaders/screentest.vert");
    screenshaderFragment.compileSourceFile("./Shaders/screentest.frag");
    programs[ShaderProgramType::SCREEN]=new QOpenGLShaderProgram();
    programs[ShaderProgramType::SCREEN]->addShader(&screenshaderVertex);
    programs[ShaderProgramType::SCREEN]->addShader(&screenshaderFragment);
    programs[ShaderProgramType::SCREEN]->bindAttributeLocation("position", 0);
    programs[ShaderProgramType::SCREEN]->link();


    QOpenGLShader outlineVertex(QOpenGLShader::Vertex);
    QOpenGLShader outlineFragment(QOpenGLShader::Fragment);
    outlineVertex.compileSourceFile("./Shaders/lineColored.vert");
    outlineFragment.compileSourceFile("./Shaders/lineColored.frag");
    programs[ShaderProgramType::OUTLINE]=new QOpenGLShaderProgram();
    programs[ShaderProgramType::OUTLINE]->addShader(&outlineVertex);
    programs[ShaderProgramType::OUTLINE]->addShader(&outlineFragment);
    programs[ShaderProgramType::OUTLINE]->bindAttributeLocation("position", 0);
    programs[ShaderProgramType::OUTLINE]->link();

    QOpenGLShader colorVertex(QOpenGLShader::Vertex);
    QOpenGLShader colorFragment(QOpenGLShader::Fragment);
    colorVertex.compileSourceFile("./Shaders/colorShader.vert");
    colorFragment.compileSourceFile("./Shaders/colorShader.frag");
    programs[ShaderProgramType::COLOR]=new QOpenGLShaderProgram();
    programs[ShaderProgramType::COLOR]->addShader(&colorVertex);
    programs[ShaderProgramType::COLOR]->addShader(&colorFragment);
    programs[ShaderProgramType::COLOR]->bindAttributeLocation("position", 0);
    programs[ShaderProgramType::COLOR]->bindAttributeLocation("normal", 1);
    programs[ShaderProgramType::COLOR]->link();

    QOpenGLShader billboardVertex(QOpenGLShader::Vertex);
    QOpenGLShader billboardFragment(QOpenGLShader::Fragment);
    billboardVertex.compileSourceFile("./Shaders/billboard.vert");
    billboardFragment.compileSourceFile("./Shaders/billboard.frag");
    programs[ShaderProgramType::BILLBOARD]=new QOpenGLShaderProgram();
    programs[ShaderProgramType::BILLBOARD]->addShader(&billboardVertex);
    programs[ShaderProgramType::BILLBOARD]->addShader(&billboardFragment);
    programs[ShaderProgramType::BILLBOARD]->bindAttributeLocation("position", 0);
    programs[ShaderProgramType::BILLBOARD]->bindAttributeLocation("in_uv", 1);
    programs[ShaderProgramType::BILLBOARD]->link();

    //position and hard normal
    float boxGeometryBuffer[]={
        //-z
        -1,-1,-1,   0,0,-1,
         1,-1,-1,   0,0,-1,
        -1, 1,-1,   0,0,-1,
        -1, 1,-1,   0,0,-1,
         1,-1,-1,   0,0,-1,
         1, 1,-1,   0,0,-1,
        //z
        -1, 1,1,    0,0,1,
         1,-1,1,    0,0,1,
        -1,-1,1,    0,0,1,
         1, 1,1,    0,0,1,
         1,-1,1,    0,0,1,
        -1, 1,1,    0,0,1,
        //-x
        -1,-1,-1,   -1,0,0,
        -1, 1,-1,   -1,0,0,
        -1,-1, 1,   -1,0,0,
        -1,-1, 1,   -1,0,0,
        -1, 1,-1,   -1,0,0,
        -1, 1, 1,   -1,0,0,
        //x
        1,-1, 1,    1,0,0,
        1, 1,-1,    1,0,0,
        1,-1,-1,    1,0,0,
        1, 1, 1,    1,0,0,
        1, 1,-1,    1,0,0,
        1,-1, 1,    1,0,0,
        //-y
        -1,-1,-1,   0,-1,0,
         1,-1,-1,   0,-1,0,
        -1,-1, 1,   0,-1,0,
        -1,-1, 1,   0,-1,0,
         1,-1,-1,   0,-1,0,
         1,-1, 1,   0,-1,0,
        //y
        -1,1, 1,    0,1,0,
         1,1,-1,    0,1,0,
        -1,1,-1,    0,1,0,
         1,1, 1,    0,1,0,
         1,1,-1,    0,1,0,
        -1,1, 1,    0,1,0
    };

    vbo[VertexBufferType::BOXGEOM]=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[VertexBufferType::BOXGEOM]->create();
    vbo[VertexBufferType::BOXGEOM]->bind();
    vbo[VertexBufferType::BOXGEOM]->allocate(boxGeometryBuffer,sizeof(float)*6*6*6);
    vao[VertexBufferType::BOXGEOM]=new QOpenGLVertexArrayObject();
    vao[VertexBufferType::BOXGEOM]->create();
    vao[VertexBufferType::BOXGEOM]->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));

    vao[VertexBufferType::BOXGEOM]->release();
    vbo[VertexBufferType::BOXGEOM]->release();

    float boxLineBuffer[]={
        //1
        -1,-1,-1,
        -1,-1,1,

        -1,-1,-1,
        -1,1,-1,

        -1,-1,-1,
        1,-1,-1,
        //2
        1,1,-1,
        1,-1,-1,

        1,1,-1,
        1,1,1,

        1,1,-1,
        -1,1,-1,
        //3
        1,-1,1,
        -1,-1,1,

        1,-1,1,
        1,-1,-1,

        1,-1,1,
        1,1,1,
        //4
        -1,1,1,
        1,1,1,

        -1,1,1,
        -1,-1,1,

        -1,1,1,
        -1,1,-1
    };
    vbo[VertexBufferType::BOXOUTLINE]=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[VertexBufferType::BOXOUTLINE]->create();
    vbo[VertexBufferType::BOXOUTLINE]->bind();
    vbo[VertexBufferType::BOXOUTLINE]->allocate(boxLineBuffer,sizeof(float)*6*3*4);
    vao[VertexBufferType::BOXOUTLINE]=new QOpenGLVertexArrayObject();
    vao[VertexBufferType::BOXOUTLINE]->create();
    vao[VertexBufferType::BOXOUTLINE]->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    vao[VertexBufferType::BOXOUTLINE]->release();
    vbo[VertexBufferType::BOXOUTLINE]->release();


    float screenArray[]={
        -1,-1,
         1,-1,
        -1, 1,
        -1, 1,
         1,-1,
         1, 1
    };

    vbo[VertexBufferType::SCREENQUAD]=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[VertexBufferType::SCREENQUAD]->create();
    vbo[VertexBufferType::SCREENQUAD]->bind();
    vbo[VertexBufferType::SCREENQUAD]->allocate(screenArray,sizeof(float)*6*2);
    vao[VertexBufferType::SCREENQUAD]=new QOpenGLVertexArrayObject();
    vao[VertexBufferType::SCREENQUAD]->create();
    vao[VertexBufferType::SCREENQUAD]->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    vao[VertexBufferType::SCREENQUAD]->release();
    vbo[VertexBufferType::SCREENQUAD]->release();


    float quadArray[]={
        -1,-1,0, 0,0,0,
         1,-1,0, 1,0,0,
        -1, 1,0, 0,1,0,
        -1, 1,0, 0,1,0,
         1,-1,0, 1,0,0,
         1, 1,0, 1,1,0
    };

    vbo[VertexBufferType::QUAD]=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[VertexBufferType::QUAD]->create();
    vbo[VertexBufferType::QUAD]->bind();
    vbo[VertexBufferType::QUAD]->allocate(quadArray,sizeof(float)*6*6);
    vao[VertexBufferType::QUAD]=new QOpenGLVertexArrayObject();
    vao[VertexBufferType::QUAD]->create();
    vao[VertexBufferType::QUAD]->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));

    vao[VertexBufferType::QUAD]->release();
    vbo[VertexBufferType::QUAD]->release();

    float skyboxArray[]={
        -1,-1,-1,
         1,-1,-1,
        -1, 1,-1,
        -1, 1,-1,
         1,-1,-1,
         1, 1,-1,

        -1, 1,1,
         1,-1,1,
        -1, -1,1,
         1, 1,1,
         1,-1,1,
        -1, 1,1,

        -1,-1,-1,
        -1, 1,-1,
        -1,-1, 1,
        -1,-1, 1,
        -1, 1,-1,
        -1, 1, 1,

        1,-1, 1,
        1, 1,-1,
        1,-1, -1,
        1, 1, 1,
        1, 1,-1,
        1,-1, 1,

        -1,-1,-1,
         1,-1,-1,
        -1,-1, 1,
        -1,-1, 1,
         1,-1,-1,
         1,-1, 1,

        -1,1, 1,
         1,1,-1,
        -1,1,-1,
         1,1, 1,
         1,1,-1,
        -1,1, 1
    };
    vbo[VertexBufferType::SKYBOXGEOM]=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[VertexBufferType::SKYBOXGEOM]->create();
    vbo[VertexBufferType::SKYBOXGEOM]->bind();
    vbo[VertexBufferType::SKYBOXGEOM]->allocate(skyboxArray,sizeof(float)*36*3);
    vao[VertexBufferType::SKYBOXGEOM]=new QOpenGLVertexArrayObject();
    vao[VertexBufferType::SKYBOXGEOM]->create();
    vao[VertexBufferType::SKYBOXGEOM]->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    vao[VertexBufferType::SKYBOXGEOM]->release();
    vbo[VertexBufferType::SKYBOXGEOM]->release();

    float containerFloat[]={
        0,0,0,
        1.5,0,0,

        0,0,0,
        0,1.5,0,

        0,0,0,
        0,0,1.5,

        1,0,1,
        1,0,0,

        1,0,1,
        1,1,1,

        1,0,1,
        0,0,1,

        1,1,0,
        1,0,0,

        1,1,0,
        1,1,1,

        1,1,0,
        0,1,0,

        0,1,1,
        0,1,0,

        0,1,1,
        0,0,1,

        0,1,1,
        1,1,1
    };
    containerCount=12*2;

    vbo[VertexBufferType::CONTAINER]=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[VertexBufferType::CONTAINER]->create();
    vbo[VertexBufferType::CONTAINER]->bind();
    vbo[VertexBufferType::CONTAINER]->allocate(containerFloat,sizeof(float)*12*3*2);
    vao[VertexBufferType::CONTAINER]=new QOpenGLVertexArrayObject();
    vao[VertexBufferType::CONTAINER]->create();
    vao[VertexBufferType::CONTAINER]->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    vao[VertexBufferType::CONTAINER]->release();
    vbo[VertexBufferType::CONTAINER]->release();

}
void BoxScene::init(QVector<BoxArrayStruct>& boxArray,Box bounds)
{
     makeCurrent();

     oXY.setPosition(QVector3D(bounds.w*0.5,bounds.h*0.5,1.5*bounds.l));
     oXY.setDirection(QVector3D(0.0f,0.0f,-1.0f));
     oXY.setUp(QVector3D(0,1,0));
     oXZ.setPosition(QVector3D(bounds.w*0.5,1.5*bounds.h,bounds.l*0.5));
     oXZ.setDirection(QVector3D(0.0f,-1.0f,0.0f));
     oXZ.setUp(QVector3D(0,0,1));
     oZY.setPosition(QVector3D(bounds.w*1.5,bounds.h*0.5,bounds.l*0.5));
     oZY.setDirection(QVector3D(-1.0f,0.0f,0.0f));
     oZY.setUp(QVector3D(0,1,0));
     oXY.setZoom(qMax(bounds.w,bounds.h));
     oXZ.setZoom(qMax(bounds.w,bounds.l));
     oZY.setZoom(qMax(bounds.l,bounds.h));

     containerModel.scale(bounds.w,bounds.h,bounds.l);

     float scale=qMax(bounds.w,qMax(bounds.h,bounds.l))*0.25;
     xModel1.translate(bounds.w+scale,0,scale);
     yModel1.translate(scale,bounds.h+scale,0);
     zModel1.translate(0,scale,bounds.l+scale);
     zModel1.rotate(90.0,0.,1.,0.);
     xModel1.scale(scale);
     yModel1.scale(scale);
     zModel1.scale(scale);

     xModel2.translate(bounds.w+scale,scale,0);
     yModel2.translate(0,bounds.h+scale,scale);
     zModel2.translate(scale,0,bounds.l+scale);
     zModel2.rotate(-90.0,1.,0.,0.);
     yModel2.rotate(90.0,0.,1.,0.);
     xModel1.rotate(90.0,1.,0.,0.);
     xModel2.scale(scale);
     yModel2.scale(scale);
     zModel2.scale(scale);

     boxCount=boxArray.length();
     float maxx=0.0f;
     float maxy=0.0f;
     float maxz=0.0f;
     for(int i=0;i<boxCount;i++)
     {
         QVector3D color;
         color.setX(boxArray[i].r/255.);
         color.setY(boxArray[i].g/255.);
         color.setZ(boxArray[i].b/255.);
         QMatrix4x4 model;
         model.translate(boxArray[i].cx,boxArray[i].cy,boxArray[i].cz);
         model.scale(boxArray[i].sx*0.5,boxArray[i].sy*0.5,boxArray[i].sz*0.5);
         maxx=qMax(maxx,boxArray[i].cx+boxArray[i].sx*0.5f);
         maxy=qMax(maxy,boxArray[i].cy+boxArray[i].sy*0.5f);
         maxz=qMax(maxz,boxArray[i].cz+boxArray[i].sz*0.5f);
         BoxRenderInfo box;
         box.model=model;
         box.color=color;
         box.selected=false;

         boxes.append(box);
     }
     /*qDebug()<<"Dimensions: "<<maxx<<", "<<maxy<<", "<<maxz;
     qDebug()<<"volume: "<<maxx*maxy*maxz;*/
     envelope.translate(maxx*0.5,maxy*0.5,maxz*0.5);
     envelope.scale(maxx*0.5,maxy*0.5,maxz*0.5);
     perspCamera.setCenter(QVector3D(maxx*0.5,maxy*0.5,maxz*0.5));
     perspCamera.setDistance(qSqrt(maxx*maxx+maxy*maxy*maxz*maxz)*1.5f);
#ifndef NEW_RENDER
     float* boxArrayFloat=new float[boxCount*8];
     for(int i=0;i<boxCount;i++)
     {
         int index=i*8;
         boxArrayFloat[index]=boxArray[i].cx;
         boxArrayFloat[index+1]=boxArray[i].cy;
         boxArrayFloat[index+2]=boxArray[i].cz;
         boxArrayFloat[index+4]=boxArray[i].sx;
         boxArrayFloat[index+5]=boxArray[i].sy;
         boxArrayFloat[index+6]=boxArray[i].sz;
         unsigned char color[4];
         color[0]=boxArray[i].r;
         color[1]=boxArray[i].g;
         color[2]=boxArray[i].b;
         color[3]=255;
         boxArrayFloat[index+7]=*(reinterpret_cast<float*>(color));
     }

     QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
     vboBoxes=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
     vboBoxes->create();
     vboBoxes->bind();
     vboBoxes->allocate(boxArrayFloat,sizeof(float)*boxCount*8);
     vaoBoxes=new QOpenGLVertexArrayObject();
     vaoBoxes->create();
     vaoBoxes->bind();
     f->glEnableVertexAttribArray(0);
     f->glEnableVertexAttribArray(1);
     f->glEnableVertexAttribArray(2);
     f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
     f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              reinterpret_cast<void *>(4 * sizeof(GLfloat)));
     f->glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_TRUE, 8 * sizeof(GLfloat),
                              reinterpret_cast<void *>(7 * sizeof(GLfloat)));

     vboBoxes->release();
     vaoBoxes->release();
     delete []boxArrayFloat;
#endif
     doneCurrent();
     initialized=true;
     float distanceNormalized=perspCamera.getDistanceNormalized();
     emit updateDistance(distanceNormalized);
}
void BoxScene::resizeGL(int w, int h)
{
    this->w=w;
    this->h=h;
}
void BoxScene::paintGL()
{
    if(!initialized)
        return;
    makeCurrent();
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    f->glClearColor(1, 1, 1, 1);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glViewport(0,0,w,h);
    Camera * currentCamera;
    bool orthoCamera=true;
    switch(cameraType)
    {
        case CameraType::FreeCamera:
            currentCamera=&perspCamera;
            orthoCamera=false;
            break;
        case OXY:
            currentCamera=&oXY;
            break;
        case OXZ:
            currentCamera=&oXZ;
            break;
        case OZY:
            currentCamera=&oZY;
            break;
    }
    currentCamera->setWidth(w);
    currentCamera->setHeight(h);
    QMatrix4x4 view=currentCamera->getView();
    QMatrix4x4 projection=currentCamera->getProjection();
    QMatrix4x4 vp=projection*view;
    f->glEnable(GL_DEPTH_TEST);
    f->glCullFace(GL_FRONT_AND_BACK);
    f->glDepthFunc(GL_LEQUAL);

    /*f->glEnable(GL_BLEND);
    f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    f->glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);//premultiplied alpha
    */
    //f->glDepthMask(GL_FALSE);

    //qDebug() << "yaw:" << camera.yaw << "pitch:" << camera.pitch;
    //render boxes
#define NEW_BOX_RENDER
#ifdef NEW_BOX_RENDER
    f->glEnable(GL_POLYGON_OFFSET_FILL);
    f->glPolygonOffset(1.0f, 2.0f);
    f->glLineWidth(2.);
    for(int i=0;i<boxes.length();i++)
    {
        BoxRenderInfo box=boxes[i];
        QMatrix4x4 model=box.model;
        QMatrix4x4 mvp=projection*view*model;
        QMatrix3x3 normalMatrix=(view*model).normalMatrix();
        //renderBox;
        programs[ShaderProgramType::COLOR]->bind();
        programs[ShaderProgramType::COLOR]->setUniformValue("mvp",mvp);
        programs[ShaderProgramType::COLOR]->setUniformValue("normalMatrix",normalMatrix);
        programs[ShaderProgramType::COLOR]->setUniformValue("alpha",0.75f);
        programs[ShaderProgramType::COLOR]->setUniformValue("color",box.color);
        vao[VertexBufferType::BOXGEOM]->bind();
        f->glDrawArrays(GL_TRIANGLES,0,36);
        vao[VertexBufferType::BOXGEOM]->release();
        programs[ShaderProgramType::COLOR]->release();
        //renderOutline;

        programs[ShaderProgramType::OUTLINE]->bind();
        programs[ShaderProgramType::OUTLINE]->setUniformValue("mvp",mvp);
        vao[VertexBufferType::BOXOUTLINE]->bind();
        if(box.selected)
        {
            f->glLineWidth(3.);
            programs[ShaderProgramType::OUTLINE]->setUniformValue("color",1.0f,0.0f,0.0f);
            f->glDrawArrays(GL_LINES,0,24);
            f->glLineWidth(2.);
        }else
        {
            programs[ShaderProgramType::OUTLINE]->setUniformValue("color",0.0f,0.0f,0.0f);
            f->glDrawArrays(GL_LINES,0,24);
        }
        programs[ShaderProgramType::OUTLINE]->release();
        vao[VertexBufferType::BOXOUTLINE]->release();
    }
    f->glDisable(GL_POLYGON_OFFSET_LINE);
    f->glLineWidth(2.);
    //render envelope
    programs[ShaderProgramType::OUTLINE]->bind();
    programs[ShaderProgramType::OUTLINE]->setUniformValue("mvp",projection*view*envelope);
    programs[ShaderProgramType::OUTLINE]->setUniformValue("color",0.0f,0.0f,1.0f);
    vao[VertexBufferType::BOXOUTLINE]->bind();
    f->glDrawArrays(GL_LINES,0,24);
    programs[ShaderProgramType::OUTLINE]->release();
    vao[VertexBufferType::BOXOUTLINE]->release();
#else
/*#ifdef RENDER_BOXES
    boxshader->bind();
    boxshader->setUniformValue("mvp",vp);
    boxshader->setUniformValue("alpha",0.75f);
    vaoBoxes->bind();
    f->glDrawArrays(GL_POINTS,0,boxCount);
    vao[VertexBufferType::BOX]->release();
    boxshader->release();
#endif*/
#endif
    f->glDisable(GL_BLEND);
#define RENDER_CONTAINER
#ifdef RENDER_CONTAINER
    QMatrix4x4 mvp;
    mvp=projection*view*containerModel;
    programs[ShaderProgramType::OUTLINE]->bind();
    programs[ShaderProgramType::OUTLINE]->setUniformValue("mvp",mvp);
    programs[ShaderProgramType::OUTLINE]->setUniformValue("color",0.25,0.25,0.25);
    vao[VertexBufferType::CONTAINER]->bind();
    f->glDrawArrays(GL_LINES,0,containerCount);
    vao[VertexBufferType::CONTAINER]->release();
    programs[ShaderProgramType::OUTLINE]->release();
#endif

#define RENDER_SKYBOX
#ifdef RENDER_SKYBOX
    if(!orthoCamera)
    {
        QMatrix4x4 skyboxMatrix;
        QMatrix4x4 _view=QMatrix4x4(view);
        _view(0,3)=0.0;
        _view(1,3)=0.0;
        _view(2,3)=0.0;
        skyboxMatrix=projection*_view;
        programs[ShaderProgramType::SKYBOX]->bind();
        programs[ShaderProgramType::SKYBOX]->setUniformValue("mvp",skyboxMatrix);
        vao[VertexBufferType::SKYBOXGEOM]->bind();
        f->glDrawArrays(GL_TRIANGLES,0,36);
        vao[VertexBufferType::SKYBOXGEOM]->release();
        programs[ShaderProgramType::SKYBOX]->release();
    }
#endif


#define RENDER_SIGNS
#ifdef RENDER_SIGNS
    if(orthoCamera)
        f->glDisable(GL_DEPTH_TEST);
    /*f->glEnable(GL_BLEND);
    f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    f->glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);*/

    programs[ShaderProgramType::BILLBOARD]->bind();
    fontTexture->bind(0);
    mvp=projection*view*xModel1;
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("charAtlas",0);
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("charID",8.f,8.f);
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("mvp",mvp);
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("color",0.f,0.f,0.f);

    vao[VertexBufferType::QUAD]->bind();
    f->glDrawArrays(GL_TRIANGLES,0,6);

    mvp=projection*view*xModel2;
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("mvp",mvp);
    f->glDrawArrays(GL_TRIANGLES,0,6);

    mvp=projection*view*yModel1;
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("charID",9.f,8.f);
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("mvp",mvp);

    f->glDrawArrays(GL_TRIANGLES,0,6);

    mvp=projection*view*yModel2;
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("mvp",mvp);
    f->glDrawArrays(GL_TRIANGLES,0,6);

    mvp=projection*view*zModel1;
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("charID",10.f,8.f);
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("mvp",mvp);

    f->glDrawArrays(GL_TRIANGLES,0,6);

    mvp=projection*view*zModel2;
    programs[ShaderProgramType::BILLBOARD]->setUniformValue("mvp",mvp);
    f->glDrawArrays(GL_TRIANGLES,0,6);

    fontTexture->release(0);
    vao[VertexBufferType::QUAD]->release();
    programs[ShaderProgramType::BILLBOARD]->release();

    f->glDisable(GL_BLEND);
#endif

#ifdef RENDER_SCREEN
    programs[ShaderProgramType::SCREEN]->bind();
    vao[VertexBufferType::SCREENQUAD]->bind();
    f->glDrawArrays(GL_TRIANGLES,0,2);
    vao[VertexBufferType::SCREENQUAD]->release();
    screenshader->release();
#endif
    doneCurrent();
}

BoxScene::BoxScene(QWidget* parent) : QOpenGLWidget(parent)
{
    mouseCaptured=false;
}
void BoxScene::mousePressEvent(QMouseEvent *event)
{
    if(event->button()&Qt::LeftButton)
    {
    setFocus();
    mouseCaptured=true;
    QGuiApplication::setOverrideCursor(QCursor(Qt::BlankCursor));

    const QRect & rect = geometry();

    qint32 x=0.5*(rect.left()+rect.right());
    qint32 y=0.5*(rect.top()+rect.bottom());
    QPoint center=mapToGlobal(QPoint(x, y));
    QCursor::setPos(center);
    lastPos = QPoint(x, y);
    }
}
void BoxScene::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()&Qt::LeftButton)
    {
        mouseCaptured=false;
        QGuiApplication::restoreOverrideCursor();
    }
}
void BoxScene::leaveEvent(QEvent *event)
{
    if(mouseCaptured)
    {
        const QRect & rect = geometry();
        QPoint position = QCursor::pos();

        qint32 x=0.5*(rect.left()+rect.right());
        qint32 y=0.5*(rect.top()+rect.bottom());
        QCursor::setPos(mapToGlobal(QPoint(x, y)));
        // Check the bounds
        /*qint32 x = qBound(rect.left(), position.x(), rect.right());
        qint32 y = qBound(rect.top(), position.y(), rect.bottom());

        // Adjust the cursor
        if (x != position.x() || y != position.y())
            QCursor::setPos(x, y);*/
        event->accept();
        update();
    }
}
void BoxScene::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseCaptured)
    {
        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();

        if(cameraType==CameraType::FreeCamera)
            perspCamera.move(QVector2D(-dx,dy)/QVector2D(width(),height()));

        const QRect & rect = geometry();

        qint32 x=0.5*(rect.left()+rect.right());
        qint32 y=0.5*(rect.top()+rect.bottom());
        QPoint center=mapToGlobal(QPoint(x, y));
        QCursor::setPos(center);
        lastPos = QPoint(x, y);
        update();
    }
}

void BoxScene::wheelEvent(QWheelEvent *event)
{
    if(mouseCaptured)
    {
        if(cameraType==CameraType::FreeCamera)
        {
            perspCamera.changeDistance(event->angleDelta().y()/24);
            float distanceNormalized=perspCamera.getDistanceNormalized();
            emit updateDistance(distanceNormalized);
            update();
        }
    }
}


void BoxScene::setDistance(float distanceNormalized)
{
    this->perspCamera.setDistance(distanceNormalized*
                                  (perspCamera.getMaxDistance()-perspCamera.getMinDistance())
                                  +perspCamera.getMinDistance());
    update();
}

void BoxScene::select(const QVector<int> &indexes)
{
    for(int i=0;i<boxes.length();i++)
        boxes[i].selected=false;
    for(int i=0;i<indexes.length();i++)
        boxes[indexes[i]].selected=true;
    update();
}

void BoxScene::setCameraType(CameraType type)
{
    cameraType=type;
}

BoxScene::~BoxScene()
{
    makeCurrent();
    for(int i=0;i<BUFFERS;i++)
    {
        delete vbo[i];
        delete vao[i];
    }
    delete fontTexture;
    for(int i=0;i<PROGRAMS;i++)
    {
        delete programs[i];
    }

    doneCurrent();
}
