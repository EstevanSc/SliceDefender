# configuration Qt
QT       += core gui

equals(QT_MAJOR_VERSION, 5) {
        QT += opengl widgets
} else:equals(QT_MAJOR_VERSION, 6) {
        QT += openglwidgets
}

TEMPLATE  = app

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

# nom de l'exe genere
TARGET 	  = slice-defender

# fichiers sources/headers
SOURCES	+= main.cpp myglwidget.cpp \
    CameraHandler.cpp \
    cannon.cpp \
    keyboardhandler.cpp \
    mainwindow.cpp \
    player.cpp \
    projectile.cpp \
    projectileManager.cpp \
    projectiles/apple.cpp \
    projectiles/appleHalf.cpp \
    projectiles/banana.cpp \
    projectiles/bananaHalf.cpp \
    projectiles/corn.cpp \
    projectiles/cornHalf.cpp \
    projectiles/orange.cpp \
    projectiles/orangeHalf.cpp \
    projectiles/strawberry.cpp \
    projectiles/strawberryHalf.cpp \
    game.cpp
HEADERS += myglwidget.h \
    CameraHandler.h \
    cannon.h \
    CameraHandler.h\
    keyboardhandler.h \
    mainwindow.h \
    player.h \
    projectile.h \
    projectileManager.h \
    projectiles/apple.h \
    projectiles/appleHalf.h \
    projectiles/banana.h \
    projectiles/bananaHalf.h \
    projectiles/corn.h \
    projectiles/cornHalf.h \
    projectiles/orange.h \
    projectiles/orangeHalf.h \
    projectiles/strawberry.h \
    projectiles/strawberryHalf.h \
    game.h

RESOURCES += \
    res/textures.qrc

DISTFILES += \
    res/apple_color.jpg \
    res/apple_normal.png \
    res/orange_color.jpg \
    res/orange_normal.png

INCLUDEPATH +=$$(OPENCV_DIR)\..\..\include

LIBS += -L$$(OPENCV_DIR)\lib -lopencv_core4110 -lopencv_highgui4110 -lopencv_imgproc4110 -lopencv_imgcodecs4110 -lopencv_videoio4110 -lopencv_features2d4110 -lopencv_calib3d4110 -lopencv_objdetect4110

FORMS += \
    camerahandler.ui \
    mainwindow.ui
