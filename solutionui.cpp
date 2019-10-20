#include "solutionui.h"
#include "ui_solutionui.h"

solutionUI::solutionUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::solutionUI)
{
    ui->setupUi(this);
    ui->openGLWidget->setCameraType(CameraType::FreeCamera);
}

solutionUI::~solutionUI()
{
    delete ui;
}

void solutionUI::setup(QVector<BoxInfo> &placements, QList<Box> &boxes,Box bounds)
{
    QVector<BoxArrayStruct> boxArray;

    for(auto iter=placements.begin();iter!=placements.end();++iter)
    {
        BoxArrayStruct boxStruct;
        Box box=boxes[iter->boxID];
        boxStruct.cx=iter->x;
        boxStruct.cy=iter->y;
        boxStruct.cz=iter->z;
        boxStruct.sx=box.w;
        boxStruct.sy=box.h;
        boxStruct.sz=box.l;
        boxStruct.r=box.color.red();
        boxStruct.g=box.color.green();
        boxStruct.b=box.color.blue();
        boxArray.push_back(boxStruct);
    }
    /*boxArray.push_back(BoxArrayStruct{0,0,0,1,2,3,255,0,0});
    boxArray.push_back(BoxArrayStruct{2,2,3,1,2,3,0,255,0});
    boxArray.push_back(BoxArrayStruct{6,6,6,1,2,3,0,0,255});*/

    ui->openGLWidget->init(boxArray,bounds);
    initList(boxArray);
}

void solutionUI::initList(QVector<BoxArrayStruct>& boxArray)
{
    ui->listWidget->clear();
    int index=0;
    for(QVector<BoxArrayStruct>::iterator iter=boxArray.begin();iter!=boxArray.end();++iter)
    {
        QString item;
        QTextStream stream(&item);
        stream<<index+1<<". "<<"center: ("<<iter->cx-0.5*iter->sx<<','<<iter->cy-0.5*iter->sy<<','<<
                iter->cz-0.5*iter->sz
             <<") sizes: ("<<iter->sx<<","<<iter->sy<<","<<iter->sz<<")";
        ui->listWidget->addItem(item);
        index++;
    }
}
void solutionUI::setLog(QString logText)
{
    ui->logTextEdit->setPlainText(logText);
}
