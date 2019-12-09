#include "helpwindow.h"
#include <QHelpEngine>
#include <QTabWidget>
#include <QSplitter>
#include <QApplication>
#include <QGridLayout>
#include "helpbrowser.h"
#include <QDebug>


HelpWindow::HelpWindow(QWidget *parent):QDialog(parent)
{
    QHelpEngine* helpEngine = new QHelpEngine(
                QApplication::applicationDirPath() +
                "/documentation/boxpacking.qhc");
    helpEngine->setupData();

    QTabWidget* tWidget = new QTabWidget;
    tWidget->setMaximumWidth(200);
    tWidget->addTab(reinterpret_cast<QWidget*>(helpEngine->contentWidget()), "Contents");
    tWidget->addTab(reinterpret_cast<QWidget*>(helpEngine->indexWidget()), "Index");

    HelpBrowser *textViewer = new HelpBrowser(helpEngine);
    textViewer->setSource(
                QUrl("qthelp://org.qt.boxpacking/doc/index.html"));
    connect(reinterpret_cast<QObject*>(helpEngine->contentWidget()),
            SIGNAL(linkActivated(QUrl)),
            textViewer, SLOT(setSource(QUrl)));
    connect(reinterpret_cast<QObject*>(helpEngine->indexWidget()),
            SIGNAL(linkActivated(QUrl, QString)),
            textViewer, SLOT(setSource(QUrl)));

    QSplitter *horizSplitter = new QSplitter(Qt::Horizontal);
    horizSplitter->insertWidget(0, tWidget);
    horizSplitter->insertWidget(1, textViewer);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(horizSplitter, 0, 0);
    setLayout(mainLayout);
    QString title="Помощь";
    setWindowTitle(title);
}
