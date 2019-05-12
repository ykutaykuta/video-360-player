QT += quick core gui opengl multimedia widgets

CONFIG += c++11
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
	src/controller.cpp \
        src/main.cpp \
    src/openglqml.cpp \
    src/ffmpegdecode.cpp \
    src/audiooutput.cpp \
    src/videooutput.cpp \
    src/videohandle.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/controller.h \
    src/openglqml.h \
    src/ffmpegdecode.h \
    src/audiooutput.h \
    src/videooutput.h \
    src/videohandle.h

INCLUDEPATH += $$PWD/dependencies/ffmpeg/include
DEPENDPATH += $$PWD/dependencies/ffmpeg/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/dependencies/ffmpeg/bin/ -llibavcodec.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/dependencies/ffmpeg/bin/ -llibavcodec.dll

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/dependencies/ffmpeg/bin/ -llibavutil.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/dependencies/ffmpeg/bin/ -llibavutil.dll

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/dependencies/ffmpeg/bin/ -llibavformat.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/dependencies/ffmpeg/bin/ -llibavformat.dll

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/dependencies/ffmpeg/bin/ -llibswscale.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/dependencies/ffmpeg/bin/ -llibswscale.dll

DEPENDPATH += $$PWD/dependencies/ffmpeg/bin

DEPENDPATH += $$PWD/dependencies/sdl2/bin

INCLUDEPATH += $$PWD/dependencies/sdl2/include
DEPENDPATH += $$PWD/dependencies/sdl2/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/dependencies/sdl2/bin/ -lSDL2.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/dependencies/sdl2/bin/ -lSDL2.dll
