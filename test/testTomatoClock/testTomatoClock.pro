QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_commdeftest.cpp \
    ../../src/commdef.cpp \
    commdef_test.cpp \
    main.cpp

HEADERS += \
    commdef_test.h \
    commdeftest.hpp
