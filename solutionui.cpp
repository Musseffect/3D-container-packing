#include "solutionui.h"
#include "ui_solutionui.h"

solutionUI::solutionUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::solutionUI)
{
    ui->setupUi(this);
    ui->openGLWidget->setCameraType(CameraType::FreeCamera);
    model=new BoxPlacementTableModel(this);
    ui->boxPlacementView->setModel(model);
    connect(
        ui->boxPlacementView->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
        this,
        SLOT(onSelectionChange(const QItemSelection &, const QItemSelection &)));
    ui->boxPlacementView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->openGLWidget,
            SIGNAL(updateDistance(float)),
            this,
            SLOT(updateSlider(float)));
}
void solutionUI::onSelectionChange(const QItemSelection &selected, const QItemSelection &deselected)
{
    QItemSelection selection=ui->boxPlacementView->selectionModel()->selection();
    QVector<int> indexes;
    for(auto iter=selection.begin();iter!=selection.end();iter++)
    {
        //qDebug()<<"bottom-top"<<iter->bottom()<<iter->top();
        //model->removeRows(iter->bottom(),1,QModelIndex());
        for(int j=iter->bottom();j<=iter->top();j++)
        {
            indexes.push_back(j);
        }
    }
    ui->openGLWidget->select(indexes);
}

solutionUI::~solutionUI()
{
    delete ui;
    delete model;
}
bool lessThanPlacement(const BoxInfo &s1, const BoxInfo &s2)
{
    return s1.boxID < s2.boxID;
}

void solutionUI::setup(QVector<BoxInfo> &placements, const QVector<Box> &boxes,const Box bounds)
{
    QVector<BoxArrayStruct> boxArray;
    qSort(placements.begin(),placements.end(),lessThanPlacement);

    QList<BoxPlacementStruct> data;
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

        data.append(BoxPlacementStruct{box.w,box.h,box.l,
                    box.color(),iter->x,iter->y,iter->z});
    }

    model->init(data);
    ui->openGLWidget->init(boxArray,bounds);
}

void solutionUI::setLog(QString logText)
{
    ui->logTextEdit->setPlainText(logText);
}

void solutionUI::on_freeCamera_clicked()
{
    ui->openGLWidget->setCameraType(CameraType::FreeCamera);
    ui->openGLWidget->update();
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

void solutionUI::on_horizontalSlider_valueChanged(int value)
{
    ui->openGLWidget->setDistance(value/100.f);
}

void solutionUI::updateSlider(float distance)
{
    ui->horizontalSlider->setValue(distance*100);
}
