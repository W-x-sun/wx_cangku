QT += core gui opengl

TARGET = hair
TEMPLATE = app


LIBS += -lglut -lGLU -lGLEW
QMAKE_CXXFLAGS += -std=c++11


INCLUDEPATH += \
    glm \
    src \
    src/lib \
    src/mike \
    src/shaderPrograms \
    src/ui

DEPENDPATH += \
    glm \
    src \
    src/lib \
    src/mike \
    src/shaderPrograms \
    src/ui

SOURCES += \
    src/ui/mainwindow.cpp \
    src/main.cpp \
    src/glwidget.cpp \
    src/lib/resourceloader.cpp \
    src/lib/openglshape.cpp \
    src/lib/errorchecker.cpp \
    src/hairobject.cpp \
    src/mike/hair.cpp \
    src/mike/simulation.cpp \
    src/shaderPrograms/shaderprogram.cpp \
    src/mike/integrator.cpp \
    src/lib/objloader.cpp \
    src/objmesh.cpp \
    src/shaderPrograms/hairshaderprogram.cpp \
    src/shaderPrograms/meshshaderprogram.cpp \
    src/ui/hairinterface.cpp \
    src/shaderPrograms/texturedquadshaderprogram.cpp \
    src/texture.cpp \
    src/framebuffer.cpp \
    src/quad.cpp \
    src/shaderPrograms/hairopacityshaderprogram.cpp \
    src/meshocttree.cpp \
    src/md5.cpp \
    src/tessellator.cpp \
    src/shaderPrograms/hairfeedbackshaderprogram.cpp \
    src/ui/sceneeditor.cpp \
    src/ui/scenewidget.cpp \
    src/lib/blurrer.cpp \
    src/mainwindow_new.cpp \
    src/ui/gong_qu.cpp \
    src/ui/sim_fun.cpp \
    src/fajixian.cpp \
    src/ui/glw.cpp


HEADERS += \
    src/ui/mainwindow.h \
    src/glwidget.h \
    src/lib/resourceloader.h \
    src/lib/openglshape.h \
    src/lib/errorchecker.h \
    src/hairobject.h \
    src/hairCommon.h \
    src/mike/hair.h \
    src/mike/simulation.h \
    src/shaderPrograms/shaderprogram.h \
    src/mike/integrator.h \
    src/lib/objloader.hpp \
    src/objmesh.h \
    src/shaderPrograms/hairshaderprogram.h \
    src/shaderPrograms/meshshaderprogram.h \
    src/ui/hairinterface.h \
    src/shaderPrograms/texturedquadshaderprogram.h \
    src/texture.h \
    src/framebuffer.h \
    src/quad.h \
    src/shaderPrograms/hairopacityshaderprogram.h \
    src/meshocttree.h \
    src/shaderPrograms/whitemeshshaderprogram.h \
    src/shaderPrograms/whitehairshaderprogram.h \
    src/md5.h \
    src/tessellator.h \
    src/shaderPrograms/hairfeedbackshaderprogram.h \
    src/shaderPrograms/hairrendershaderprogram.h \
    src/ui/sceneeditor.h \
    src/ui/scenewidget.h \
    src/lib/blurrer.h \
    src/shaderPrograms/hairdepthpeelprogram.h \
    src/shaderPrograms/meshdepthpeelprogram.h \
    src/mainwindow_new.h \
    src/ui/gong_qu.h \
    src/ui/sim_fun.h \
    src/fajixian.h \
    src/ui/glw.h


FORMS += src/mainwindow.ui \
    src/ui/sceneeditor.ui \
    src/mainwindow_new.ui \
    src/ui/gong_qu.ui \
    src/ui/sim_fun.ui \
    src/fajixian.ui \
    src/ui/glw.ui

OTHER_FILES += \
    shaders/mesh.frag \
    shaders/mesh.vert \
    shaders/hair.frag \
    shaders/hair.geom \
    shaders/hair.tcs \
    shaders/hair.tes \
    shaders/hair.vert \
    shaders/texturedquad.vert \
    shaders/texturedquad.frag \
    shaders/hairOpacity.frag \
    shaders/white.frag \
    shaders/constants.glsl \
    shaders/opacitymapping.glsl \
    shaders/hairrender.vert \
    shaders/depthpeel.glsl \
    shaders/hairDepthPeel.frag \
    shaders/hairlighting.glsl \
    shaders/meshlighting.glsl \
    shaders/meshdepthpeel.frag \
    shaders/hairFeedback.geom \
    shaders/hairFeedback.tes


RESOURCES += \
    shaders/shaders.qrc \
    models/models.qrc \
    images/images.qrc

QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas

