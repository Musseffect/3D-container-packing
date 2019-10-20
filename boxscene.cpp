
#include "boxscene.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>

void BoxScene::initializeGL()
{
    initialized=false;

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    fontTexture=new QOpenGLTexture(QImage(QString("./Resources/tex21.png")).mirrored());

/*
    QOpenGLShader shaderBoxVertex(QOpenGLShader::Vertex);
    shaderBoxVertex.compileSourceFile("./Shaders/boxshader.vert");
    QOpenGLShader shaderBoxGeometry(QOpenGLShader::Geometry);
    shaderBoxGeometry.compileSourceFile("./Shaders/boxshader.geom");
    QOpenGLShader shaderBoxFragment(QOpenGLShader::Fragment);
    shaderBoxFragment.compileSourceFile("./Shaders/boxshader.frag");
    boxshader=new QOpenGLShaderProgram();
    boxshader->addShader(&shaderBoxVertex);
    boxshader->addShader(&shaderBoxGeometry);
    boxshader->addShader(&shaderBoxFragment);
    boxshader->bindAttributeLocation("in_coordinates", 0);
    boxshader->bindAttributeLocation("in_size", 1);
    boxshader->bindAttributeLocation("in_color", 2);
    boxshader->link();

    containershader=new QOpenGLShaderProgram();
    QOpenGLShader containerVertex(QOpenGLShader::Vertex);
    QOpenGLShader containerFragment(QOpenGLShader::Fragment);
    containerVertex.compileSourceFile("./Shaders/container.vert");
    containerFragment.compileSourceFile("./Shaders/container.frag");
    containershader->addShader(&containerVertex);
    containershader->addShader(&containerFragment);
    containershader->bindAttributeLocation("position", 0);
    containershader->link();

    QOpenGLShader skyboxshaderVertex(QOpenGLShader::Vertex);
    QOpenGLShader skyboxshaderFragment(QOpenGLShader::Fragment);
    skyboxshaderVertex.compileSourceFile("./Shaders/skybox.vert");
    skyboxshaderFragment.compileSourceFile("./Shaders/skybox.frag");

    QOpenGLShader screenshaderVertex(QOpenGLShader::Vertex);
    QOpenGLShader screenshaderFragment(QOpenGLShader::Fragment);
    screenshaderVertex.compileSourceFile("./Shaders/screentest.vert");
    screenshaderFragment.compileSourceFile("./Shaders/screentest.frag");

    skyboxshader=new QOpenGLShaderProgram();
    skyboxshader->addShader(&skyboxshaderVertex);
    skyboxshader->addShader(&skyboxshaderFragment);
    skyboxshader->bindAttributeLocation("vertex", 0);
    skyboxshader->link();

    screenshader=new QOpenGLShaderProgram();
    screenshader->addShader(&screenshaderVertex);
    screenshader->addShader(&screenshaderFragment);
    screenshader->bindAttributeLocation("position", 0);
    screenshader->link();

    float boxArray[]={
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
        -1,1, -1,
         1,1, 1,
         1,1,-1,
        -1,1, 1
    };
    vboSkybox=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vboSkybox->create();
    vboSkybox->bind();
    vboSkybox->allocate(boxArray,sizeof(float)*36*3);
    vaoSkybox=new QOpenGLVertexArrayObject();
    vaoSkybox->create();
    vaoSkybox->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    vaoSkybox->release();
    vboSkybox->release();

    float screenArray[]={
        -1,-1,
         1,-1,
        -1, 1,
        -1, 1,
         1,-1,
         1, 1
    };

    vboScreen=new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vboScreen->create();
    vboScreen->bind();
    vboScreen->allocate(screenArray,sizeof(float)*6*2);
    vaoScreen=new QOpenGLVertexArrayObject();
    vaoScreen->create();
    vaoScreen->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    vaoScreen->release();
    vboScreen->release();


    vboContainer=new QOpenGLBuffer();
    vboContainer->create();
    vboContainer->bind();
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
    vboContainer->allocate(containerFloat,sizeof(float)*12*3*2);
    vaoContainer=new QOpenGLVertexArrayObject();
    vaoContainer->create();
    vaoContainer->bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    vboContainer->release();
    vaoContainer->release();
*/



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
    //qDebug() <<programs[ShaderProgramType::COLOR]->log();
/*
    QOpenGLShader shaderBoxVertex(QOpenGLShader::Vertex);
    shaderBoxVertex.compileSourceFile("./Shaders/boxshader.vert");
    QOpenGLShader shaderBoxGeometry(QOpenGLShader::Geometry);
    shaderBoxGeometry.compileSourceFile("./Shaders/boxshader.geom");
    QOpenGLShader shaderBoxFragment(QOpenGLShader::Fragment);
    shaderBoxFragment.compileSourceFile("./Shaders/boxshader.frag");
    boxshader=new QOpenGLShaderProgram();
    boxshader->addShader(&shaderBoxVertex);
    boxshader->addShader(&shaderBoxGeometry);
    boxshader->addShader(&shaderBoxFragment);
    boxshader->bindAttributeLocation("in_coordinates", 0);
    boxshader->bindAttributeLocation("in_size", 1);
    boxshader->bindAttributeLocation("in_color", 2);
    boxshader->link();*/

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

     oXY.setPosition(QVector3D(bounds.w*0.5,bounds.h*0.5,-0.05f));
     oXY.setDirection(QVector3D(0.0f,0.0f,1.0f));
     oXZ.setPosition(QVector3D(bounds.w*0.5,-0.05f,bounds.l*0.5));
     oXZ.setDirection(QVector3D(0.0f,1.0f,0.0f));
     oZY.setPosition(QVector3D(-0.05f,bounds.h*0.5,bounds.l*0.5));
     oZY.setDirection(QVector3D(1.0f,0.0f,0.0f));

     containerModel.scale(bounds.w,bounds.h,bounds.l);

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

         boxes.append(box);
     }
     envelope.translate(maxx*0.5,maxy*0.5,maxz*0.5);
     envelope.scale(maxx*0.5,maxy*0.5,maxz*0.5);
     perspCamera.setCenter(QVector3D(maxx*0.5,maxy*0.5,maxz*0.5));
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

    f->glClearColor(0, 0, 0, 1);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glViewport(0,0,w,h);
    Camera * currentCamera;
    switch(cameraType)
    {
        case CameraType::FreeCamera:
            currentCamera=&perspCamera;
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
        programs[ShaderProgramType::OUTLINE]->setUniformValue("color",0.0f,0.0f,0.0f);
        vao[VertexBufferType::BOXOUTLINE]->bind();
        f->glDrawArrays(GL_LINES,0,24);
        programs[ShaderProgramType::OUTLINE]->release();
        vao[VertexBufferType::BOXOUTLINE]->release();
    }
    f->glDisable(GL_POLYGON_OFFSET_LINE);
    //render envelope
    programs[ShaderProgramType::OUTLINE]->bind();
    programs[ShaderProgramType::OUTLINE]->setUniformValue("mvp",projection*view*envelope);
    programs[ShaderProgramType::OUTLINE]->setUniformValue("color",0.0f,0.0f,0.0f);
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
    mouseCaptured=true;
    QGuiApplication::setOverrideCursor(QCursor(Qt::BlankCursor));

    const QRect & rect = geometry();

    qint32 x=0.5*(rect.left()+rect.right());
    qint32 y=0.5*(rect.top()+rect.bottom());
    QPoint center=mapToGlobal(QPoint(x, y));
    QCursor::setPos(center);
    lastPos = QPoint(x, y);
}
void BoxScene::mouseReleaseEvent(QMouseEvent *event)
{
    mouseCaptured=false;
    QGuiApplication::restoreOverrideCursor();
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
            perspCamera.changeDistance(event->angleDelta().y()/24);
        update();
    }
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
