CONFIG += release
QT += network
TEMPLATE = app
TARGET = ../QNetSoul
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
    headers/Url.h \
    headers/State.h \
    headers/ContactsWriter.h \
    headers/ContactsReader.h \
    headers/InputTextEdit.h \
    headers/LocationResolver.h \
    headers/PortraitResolver.h \
    headers/Smileys.h \
    headers/VieDeMerde.h \
    headers/PortraitRequest.h \
    headers/ChuckNorrisFacts.h \
    headers/Encryption.h \
    headers/Pastebin.h \
    headers/TrayIcon.h \
    headers/OptionsWidget.h \
    headers/OptionsMainWidget.h \
    headers/OptionsChatWidget.h \
    headers/OptionsBlockedWidget.h \
    headers/OptionsFunWidget.h \
    headers/OptionsAdvancedWidget.h \
    headers/Popup.h \
    headers/SlidingPopup.h \
    headers/Randn.h \
    headers/ContactsTree.h \
    headers/ToolTipBuilder.h \
    headers/InternUpdater.h

FORMS += ui/QNetsoul.ui \
    ui/Options.ui \
    ui/AddContact.ui \
    ui/Chat.ui \

SOURCES += src/main.cpp \
    src/QNetsoul.cpp \
    src/Options.cpp \
    src/Network.cpp \
    src/AddContact.cpp \
    src/Chat.cpp \
    src/Url.cpp \
    src/ContactsWriter.cpp \
    src/ContactsReader.cpp \
    src/InputTextEdit.cpp \
    src/LocationResolver.cpp \
    src/PortraitResolver.cpp \
    src/VieDeMerde.cpp \
    src/ChuckNorrisFacts.cpp \
    src/Pastebin.cpp \
    src/Encryption.cpp \
    src/TrayIcon.cpp \
    src/OptionsMainWidget.cpp \
    src/OptionsChatWidget.cpp \
    src/OptionsBlockedWidget.cpp \
    src/OptionsFunWidget.cpp \
    src/OptionsAdvancedWidget.cpp \
    src/Popup.cpp \
    src/SlidingPopup.cpp \
    src/Randn.cpp \
    src/ContactsTree.cpp \
    src/ToolTipBuilder.cpp \
    src/InternUpdater.cpp

# Output
DESTDIR = .
OBJECTS_DIR = obj
MOC_DIR = moc
UI_SOURCES_DIR = ui
UI_HEADERS_DIR = headers
win32:RC_FILE = Windows/windowsIcon.rc
