TEMPLATE = app
CONFIG += release

#TEMPLATE = lib
#CONFIG += plugin static release

DEPENDPATH += . \
headers \
src \
tpl \
ui \
../../qns/headers \
../../qns/interfaces \

INCLUDEPATH += . \
headers \
tpl \
../../qns/headers \
../../qns/interfaces \

# Inputs
#RESOURCES += Images.qrc
FORMS += ui/pluginsmanager.ui

HEADERS += headers/pluginsmanager.h \
tpl/* \
../../qns/headers/Popup.h \
../../qns/headers/SlidingPopup.h \
../../qns/interfaces/* \


SOURCES += src/main.cpp \
src/pluginsmanager.cpp \
../../qns/src/Popup.cpp \
../../qns/src/SlidingPopup.cpp \

# Output
TARGET = PluginsManager
OBJECTS_DIR = obj
MOC_DIR = moc
UI_SOURCES_DIR = ui
UI_HEADERS_DIR = headers

unix:  DESTDIR = .
win32: DESTDIR = .
macx:  DESTDIR = .
