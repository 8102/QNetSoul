CONFIG += release
TEMPLATE = app
QT += network

DEPENDPATH += . \
    headers \
    tpl \
    src \
    ui \
    ../tools

INCLUDEPATH += . \
    headers \
    tpl \
    ../tools

# Inputs
FORMS += ui/Updater.ui ui/CredentialsDialog.ui

HEADERS += headers/Updater.h \
headers/Credentials.h \
headers/CredentialsDialog.h

SOURCES += src/main.cpp \
src/Updater.cpp \
src/Credentials.cpp \
src/CredentialsDialog.cpp

# Common inputs
SOURCES += ../tools/tools.cpp
HEADERS += ../tools/tools.h

# Output
TARGET = Updater
OBJECTS_DIR = obj
MOC_DIR = moc
UI_SOURCES_DIR = ui
UI_HEADERS_DIR = headers

unix:  DESTDIR = ../
win32: DESTDIR = ../
macx:  DESTDIR = .

win32: RC_FILE = appIcon/windows.rc
#macx:  ICON = appIcon/updater.icns
