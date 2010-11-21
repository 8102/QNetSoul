TEMPLATE = lib
CONFIG += plugin release
QT += network

DEPENDPATH += . \
headers \
src \
../../tools \
../../qns/interfaces

INCLUDEPATH += . \
headers \
../../tools \
../../qns/interfaces

# Inputs
RESOURCES += pix.qrc

HEADERS += headers/viedemerdeplugin.h \
../../qns/interfaces/ipopupplugin.h \
../../qns/interfaces/iplugindescriptor.h \

SOURCES += src/viedemerdeplugin.cpp

# Common inputs
HEADERS += ../../tools/tools.h
SOURCES += ../../tools/tools.cpp

# Output
TARGET = VieDeMerde
OBJECTS_DIR = obj
MOC_DIR = moc
UI_SOURCES_DIR = ui
UI_HEADERS_DIR = headers

unix:  DESTDIR = .
win32: DESTDIR = .
macx:  DESTDIR = .
