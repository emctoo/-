QT -= gui
QT += core

QMAKE_CC=clang

QMAKE_CXX=clang++
QMAKE_CXXFLAGS += -std=c++0x -g

QMAKE_LINK=clang++

SOURCES = main.cc

INCLUDEPATH += /depends/include /depends/include/chardet
LIBS = -lglog -lcppformat -L/depends/lib -lchardet -liconv
