# configuration Qt
QT       += core gui

equals(QT_MAJOR_VERSION, 5) {
        QT += opengl widgets
} else:equals(QT_MAJOR_VERSION, 6) {
        QT += openglwidgets
}
# ajout des libs au linker
win32 {
    win32-msvc* {
        LIBS     += opengl32.lib glu32.lib
    } else {
        LIBS     += -lopengl32 -lglu32
    }
} else:macx {
        LIBS     += -framework OpenGL
} else {
        LIBS     += -lGL -lGLU
}


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    myglwidget.cpp \
    CameraHandler.cpp \
    cannon.cpp

HEADERS += \
    mainwindow.h \
    myglwidget.h \
    CameraHandler.h \
    cannon.h

FORMS += \
    camerahandler.ui \
    mainwindow.ui


INCLUDEPATH +=$$(OPENCV_DIR)\..\..\include

LIBS += -L$$(OPENCV_DIR)\lib -lopencv_core4110 -lopencv_highgui4110 -lopencv_imgproc4110 -lopencv_imgcodecs4110 -lopencv_videoio4110 -lopencv_features2d4110 -lopencv_calib3d4110 -lopencv_objdetect4110
