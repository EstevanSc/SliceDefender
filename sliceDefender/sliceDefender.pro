QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    widget.cpp

HEADERS += \
    mainwindow.h \
    widget.h

FORMS += \
    mainwindow.ui \
    widget.ui

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target

INCLUDEPATH +=$$(OPENCV_DIR)\..\..\include

LIBS += -L$$(OPENCV_DIR)\lib -lopencv_core4110 -lopencv_highgui4110 -lopencv_imgproc4110 -lopencv_imgcodecs4110 -lopencv_videoio4110 -lopencv_features2d4110 -lopencv_calib3d4110 -lopencv_objdetect4110

