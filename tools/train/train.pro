TARGET = train

TEMPLATE = app
CONFIG += silent
QT = core gui

VPATH += ../../src ../../src/filters
DEPENDPATH += ../../src ../../src/filters
INCLUDEPATH += ../../src ../../src/filters

# Input

HEADERS += idl.h \
    rgbv.h \
    matrix.h \
    gradient.h \
    histogram.h \
    colorcorrect.h \
    gradhistogram.h \
    hog.h \
    classify.h \
    ../../src/filters/median.h
SOURCES += idl.cpp \
    gradient.cpp \
    histogram.cpp \
    colorcorrect.cpp \
    gradhistogram.cpp \
    hog.cpp \
    main_train.cpp \
    classify.cpp \
    ../../src/filters/median.cpp

# liblinear dependency

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../3rdparty/liblinear/release/ -llinear
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../3rdparty/liblinear/debug/ -llinear
else:symbian: LIBS += -llinear
else:unix: LIBS += -L$$OUT_PWD/../../3rdparty/liblinear/ -llinear

INCLUDEPATH += $$PWD/../../3rdparty/liblinear/liblinear-1.8
DEPENDPATH += $$PWD/../../3rdparty/liblinear/liblinear-1.8

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/liblinear/release/linear.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/liblinear/debug/linear.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/liblinear/liblinear.a









