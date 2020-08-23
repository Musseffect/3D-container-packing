#include "solutionDialog.h"
#include "ui_solutiondialog.h"

SolutionDialog::SolutionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolutionDialog)
{
    ui->setupUi(this);
    ui->openGLWidget->setCameraType(CameraType::FreeCamera);
    model=new BoxPositionTableModel(this);
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

void SolutionDialog::onSelectionChange(const QItemSelection &selected, const QItemSelection &deselected)
{
    QItemSelection selection=ui->boxPlacementView->selectionModel()->selection();
    QVector<int> indexes;
    for(auto iter=selection.begin();iter!=selection.end();iter++)
    {
        for(int j=iter->bottom();j<=iter->top();j++)
        {
            indexes.push_back(j);
        }
    }
    ui->openGLWidget->select(indexes);
}

SolutionDialog::~SolutionDialog()
{
    delete ui;
    delete model;
}
bool lessThanPlacement(const BoxInfo &s1, const BoxInfo &s2)
{
    return s1.boxID < s2.boxID;
}

void SolutionDialog::setup(QVector<BoxInfo> &placements, const QVector<Box> &boxes,const Box bounds)
{
    QVector<BoxArrayStruct> boxArray;
    qSort(placements.begin(),placements.end(),lessThanPlacement);

    QList<BoxPositionStruct> data;
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

        data.append(BoxPositionStruct{box.w,box.h,box.l,
                    box.color(),iter->x,iter->y,iter->z});
    }

    model->init(data);
    ui->openGLWidget->init(boxArray,bounds);
}

void SolutionDialog::setLog(QString logText)
{
    ui->logTextEdit->setPlainText(logText);
}

void SolutionDialog::on_freeCamera_clicked()
{
    ui->openGLWidget->setCameraType(CameraType::FreeCamera);
    ui->openGLWidget->update();
}

void SolutionDialog::on_oxy_clicked()
{
    ui->openGLWidget->setCameraType(CameraType::OXY);
    ui->openGLWidget->update();
}

void SolutionDialog::on_oxz_clicked()
{
    ui->openGLWidget->setCameraType(CameraType::OXZ);
    ui->openGLWidget->update();
}

void SolutionDialog::on_ozy_clicked()
{
    ui->openGLWidget->setCameraType(CameraType::OZY);
    ui->openGLWidget->update();
}

void SolutionDialog::on_horizontalSlider_valueChanged(int value)
{
    ui->openGLWidget->setDistance(value/100.f);
}

void SolutionDialog::updateSlider(float distance)
{
    ui->horizontalSlider->setValue(distance*100);
}
