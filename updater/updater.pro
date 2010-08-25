CONFIG += release
QT += network
TEMPLATE = app
TARGET = ../Updater
DEPENDPATH += . \
    headers \
    src \
    ui
INCLUDEPATH += . \
    headers

# Input
FORMS += ui/Updater.ui
HEADERS += headers/Updater.h
SOURCES += src/main.cpp src/Updater.cpp

# Output
DESTDIR = .
OBJECTS_DIR = obj
MOC_DIR = moc
UI_SOURCES_DIR = ui
UI_HEADERS_DIR = headers
