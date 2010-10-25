# -------------------------------------------------
# Project created by QtCreator 2010-04-19T15:23:18
# -------------------------------------------------
QT += network \
    webkit \
    multimedia
TARGET = Mobivu
TEMPLATE = app
INCLUDEPATH += $${EPOCROOT}epoc32\include\mmf\common \
    ffmpeg\ffmpeg\libavcodec \
    ffmpeg\ffmpeg\libavformat \
    ffmpeg\ffmpeg\libswscale \
    ffmpeg\ffmpeg\libavutil \
    ffmpeg-symbian\headers \
    ffmpeg\libjpeg
LIBS += -lecam \
    -lhwrmlightclient \
    -lcamcmediarecorder \
    -lmmfcontrollerframework \
    -lcommdb \
    -lmediaclientaudiostream \
    -lmediaclientaudio \
    -lfbscli \
    -lecom \
    -lavkon \
    -leikcdlg \
    -leikctl \
    -lcone \
    -lmediaclientaudioinputstream \
    -lefsrv \
    -lestor \
    -llibswscale.lib \
    -llibavcodec.lib \
    -llibavutil.lib \
    -llibjpeg.lib \
    -lestlib \
    -lesock \
    -linsock \
    -lplatformenv \
    -lcharconv \
    -lbafl
SOURCES += main.cpp \
    mainwindow.cpp \
    Sink.cpp \
    AudioStreamEngine.cpp \
    FrameWidget.cpp \
    ffdecode.cpp \
    mycamera.cpp \
    threadudp.cpp
HEADERS += MySleep.h \
    mainwindow.h \
    Sink.h \
    AudioStreamEngine.h \
    FrameWidget.h \
    ffdecode.h \
    sym_iap_util.h \
    mycamera.h \
    threadudp.h \
    MySleep.h
FORMS += mainwindow.ui
RESOURCES += mobivu.qrc
ICON = icon.svg
symbian::TARGET.CAPABILITY = "UserEnvironment NetworkServices"
symbian::TARGET.EPOCSTACKSIZE = 80000
symbian::TARGET.EPOCHEAPSIZE = 40 \
    10000000
