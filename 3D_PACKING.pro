#-------------------------------------------------
#
# Project created by QtCreator 2019-09-12T23:55:31
#
#-------------------------------------------------

QT       += core gui
QT += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3D_PACKING
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    boxscene.cpp \
    solutionui.cpp \
    solvers.cpp \
    camera.cpp \
    geneticsolverdialog.cpp \
    random.cpp \
    boxtablemodel.cpp \
    boxtabledelegate.cpp \
    colordialogwidget.cpp \
    greedysolverdialog.cpp \
    greedysolver.cpp \
    geneticsolver.cpp \
    boxpackingsolver.cpp \
    structs.cpp

HEADERS  += window.h \
    boxscene.h \
    solutionui.h \
    solvers.h \
    camera.h \
    geneticsolverdialog.h \
    random.h \
    boxtablemodel.h \
    structs.h \
    boxtabledelegate.h \
    colordialogwidget.h \
    greedysolverdialog.h \
    greedysolver.h \
    geneticsolver.h \
    boxpackingsolver.h

FORMS    += window.ui \
    solutionui.ui \
    geneticsolverdialog.ui \
    greedysolverdialog.ui

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
    Shaders/lineColored.frag

win32 {
    build_pass: CONFIG(debug, debug|release) {
        DESTDIR = $$OUT_PWD/debug
    }
    else: build_pass {
        DESTDIR = $$OUT_PWD/release
    }
}


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
