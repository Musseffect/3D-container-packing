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
bool lessThanPlacement(const BoxInfo &s1, const BoxInfo &s2)
{
    return s1.boxID < s2.boxID;
}

void solutionUI::setup(QVector<BoxInfo> &placements, QVector<Box> &boxes,Box bounds)
{
    QVector<BoxArrayStruct> boxArray;
    qSort(placements.begin(),placements.end(),lessThanPlacement);

    ui->listWidget->clear();
    for(auto iter=placements.begin();iter!=placements.end();++iter)
    {
        BoxArrayStruct boxStruct;
        Box box=boxes[iter->boxID].getOrientation(iter->o);
        boxStruct.cx=iter->x;
        boxStruct.cy=iter->y;
        boxStruct.cz=iter->z;
        boxStruct.sx=box.width();
        boxStruct.sy=box.height();
        boxStruct.sz=box.length();
        boxStruct.r=box.color().red();
        boxStruct.g=box.color().green();
        boxStruct.b=box.color().blue();
        boxArray.push_back(boxStruct);

        QString item;
        QTextStream stream(&item);
        stream<<iter->boxID<<". "<<"center: ("<<boxStruct.cx-0.5*boxStruct.sx
             <<','<<boxStruct.cy-0.5*boxStruct.sy<<','<<
                boxStruct.cz-0.5*boxStruct.sz
             <<") sizes: ("<<boxStruct.sx<<","<<boxStruct.sy<<","<<boxStruct.sz<<")";
        stream<<" Orientation: "<<orientationToString(iter->o);
        ui->listWidget->addItem(item);
    }

    ui->openGLWidget->init(boxArray,bounds);
    //initList(boxArray);
}

void solutionUI::initList(QVector<BoxArrayStruct>& boxArray)
{
    ui->listWidget->clear();
    int index=0;
    for(QVector<BoxArrayStruct>::iterator iter=boxArray.begin();iter!=boxArray.end();++iter)
    {
        QString item;
        QTextStream stream(&item);
        stream<<index<<". "<<"center: ("<<iter->cx-0.5*iter->sx<<','<<iter->cy-0.5*iter->sy<<','<<
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

void solutionUI::on_freeCamera_clicked()
{
    ui->openGLWidget->setCameraType(CameraType::FreeCamera);
}

void solutionUI::on_oxy_clicked()
{
    ui->openGLWidget->setCameraType(CameraType::OXY);
    ui->openGLWidget->update();
}

void solutionUI::on_oxz_clicked()
{
    ui->openGLWidget->setCameraType(CameraType::OXZ);
    ui->openGLWidget->update();
}

void solutionUI::on_ozy_clicked()
{
    ui->openGLWidget->setCameraType(CameraType::OZY);
    ui->openGLWidget->update();
}
