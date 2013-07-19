QT -= gui
CONFIG += console debug
TEMPLATE = app
TARGET = 
DEPENDPATH += . old
INCLUDEPATH += .

MOC_DIR                =  $$PWD/tmp
OBJECTS_DIR            =  $$PWD/tmp

# Input
HEADERS += src/huffman_table.h src/huffman_aux.h
SOURCES += src/main.cpp src/huffman_aux.cpp
