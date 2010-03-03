QT += network
TEMPLATE = app
TARGET = QNetSoul
DEPENDPATH += . \
    headers \
    src \
    ui
INCLUDEPATH += . \
    headers

# Input
RESOURCES += Images.qrc
HEADERS += headers/QNetsoul.h \
    headers/Network.h \
    headers/Options.h \
    headers/AddContact.h \
    headers/Chat.h \
    headers/RessourcesManager.h \
    headers/Url.h \
    headers/ContactWidget.h \
    headers/State.h \
    headers/ConnectionPoint.h \
    headers/ContactsWriter.h \
    headers/ContactsReader.h \
    headers/Contact.h \
    headers/InputTextEdit.h \
    headers/LocationResolver.h \
    headers/PortraitResolver.h \
    headers/Smileys.h \
    headers/VieDeMerde.h \
    headers/PortraitRequest.h \
    headers/Status.h \
    headers/ChuckNorrisFacts.h \
    headers/Randn.h
FORMS += ui/QNetsoul.ui \
    ui/Options.ui \
    ui/AddContact.ui \
    ui/Chat.ui \
    ui/ContactWidget.ui
SOURCES += src/main.cpp \
    src/QNetsoul.cpp \
    src/Options.cpp \
    src/Network.cpp \
    src/AddContact.cpp \
    src/Chat.cpp \
    src/RessourcesManager.cpp \
    src/Url.cpp \
    src/ContactWidget.cpp \
    src/ContactsWriter.cpp \
    src/ContactsReader.cpp \
    src/InputTextEdit.cpp \
    src/LocationResolver.cpp \
    src/PortraitResolver.cpp \
    src/VieDeMerde.cpp \
    src/ChuckNorrisFacts.cpp \
    src/Randn.cpp

# Output
DESTDIR = .
OBJECTS_DIR = obj
MOC_DIR = moc
UI_SOURCES_DIR = ui
UI_HEADERS_DIR = headers
win32:RC_FILE = Windows/windowsIcon.rc
