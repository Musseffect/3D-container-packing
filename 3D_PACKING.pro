#-------------------------------------------------
#
# Project created by QtCreator 2019-09-12T23:55:31
#
#-------------------------------------------------


QT       += core gui
QT += opengl
QT += help

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3D_PACKING
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        gui/window.cpp \
    camera.cpp \
    random.cpp \
    structs.cpp \
    solverThread.cpp \
    boxPacking/geneticHeuristicSolver.cpp \
    boxPacking/boxPackingSolver.cpp \
    boxPacking/greedySolver.cpp \
    gui/helpWindow.cpp \
    gui/helpBrowser.cpp \
    gui/solutionDialog.cpp \
    sceneGLWidget.cpp \
    gui/geneticHeuristicDialog.cpp \
    gui/geneticRpnDialog.cpp \
    gui/greedyDialog.cpp \
    boxPacking/bruteForceSolver.cpp \
    boxPacking/geneticRpnSolver.cpp \
    gui/colorDialogWidget.cpp \
    gui/bruteForceDialog.cpp \
    gui/boxTableModel.cpp \
    gui/boxTableDelegate.cpp \
    gui/boxPositionTableModel.cpp

HEADERS  += gui/window.h \
    camera.h \
    random.h \
    structs.h \
    boxPacking/bruteForceSolver.h \
    gui/colorDialogWidget.h \
    gui/geneticRpnDialog.h \
    gui/boxTableModel.h \
    gui/boxTableDelegate.h \
    gui/boxPositionTableModel.h \
    boxPacking/boxPackingSolver.h \
    boxPacking/greedySolver.h \
    gui/bruteForceDialog.h \
    gui/greedyDialog.h \
    gui/helpBrowser.h \
    gui/helpWindow.h \
    boxPacking/geneticHeuristicSolver.h \
    sceneGLWidget.h \
    solverThread.h \
    gui/solutionDialog.h \
    boxPacking/geneticRpnSolver.h \
    gui/geneticHeuristicDialog.h

FORMS    += gui/window.ui \
    gui/bruteForceDialog.ui \
    gui/geneticHeuristicDialog.ui \
    gui/geneticRpnDialog.ui \
    gui/greedyDialog.ui \
    gui/solutionDialog.ui

DISTFILES += \
    Shaders/boxshader.geom \
    Shaders/boxshader.frag \
    Shaders/boxshader.vert \
    Shaders/screentest.frag \
    Shaders/screentest.vert \
    Shaders/skybox.vert \
    Shaders/skybox.frag \
    Resources/tex21.png \
    Shaders/colorShader.frag \
    Shaders/colorShader.vert \
    Shaders/lineColored.vert \
    Shaders/lineColored.frag \
    Shaders/billboard.frag \
    Shaders/billboard.vert

win32 {
    build_pass: CONFIG(debug, debug|release) {
        DESTDIR = $$OUT_PWD/debug
    }
    else: build_pass {
        DESTDIR = $$OUT_PWD/release
    }
}
#QMAKE_CXXFLAGS += /std:c++11

##createdir.commands = $(MKDIR) $$OUT_PWD/Shaders
#copydata.commands = $(COPY_DIR) $$PWD/Shaders $$OUT_PWD/Shaders
#first.depends = $(first) createdir copydata
#export(first.depends)
#export(createdir.commands)
#export(copydata.commands)
#QMAKE_EXTRA_TARGETS += first createdir copydata
#QMAKE_POST_LINK += $(COPY_DIR) $$PWD/Shaders $$OUT_PWD/debug/Shaders $$escape_expand(\\n\\t)
QMAKE_POST_LINK += $(COPY_DIR) $$shell_path($$PWD/Shaders) $$shell_path($$DESTDIR/Shaders) $$escape_expand(\\n\\t)
QMAKE_POST_LINK += $(COPY_DIR) $$shell_path($$PWD/Resources) $$shell_path($$DESTDIR/Resources) $$escape_expand(\\n\\t)
