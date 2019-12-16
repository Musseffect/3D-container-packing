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
        window.cpp \
    boxscene.cpp \
    solvers.cpp \
    camera.cpp \
    random.cpp \
    boxtablemodel.cpp \
    boxtabledelegate.cpp \
    colordialogwidget.cpp \
    greedysolver.cpp \
    geneticsolver.cpp \
    boxpackingsolver.cpp \
    structs.cpp \
    boxplacementtablemodel.cpp \
    solverworker.cpp \
    helpwindow.cpp \
    helpbrowser.cpp \
    bruteforcesolver.cpp \
    greedydialog.cpp \
    solutiondialog.cpp \
    geneticdialog.cpp \
    bruteforcedialog.cpp

HEADERS  += window.h \
    boxscene.h \
    solvers.h \
    camera.h \
    random.h \
    boxtablemodel.h \
    structs.h \
    boxtabledelegate.h \
    colordialogwidget.h \
    greedysolver.h \
    geneticsolver.h \
    boxpackingsolver.h \
    boxplacementtablemodel.h \
    solverworker.h \
    helpwindow.h \
    helpbrowser.h \
    bruteforcesolver.h \
    solutiondialog.h \
    greedydialog.h \
    geneticdialog.h \
    bruteforcedialog.h

FORMS    += window.ui \
    greedydialog.ui \
    geneticdialog.ui \
    solutiondialog.ui \
    bruteforcedialog.ui

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
