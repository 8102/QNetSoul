CONFIG += release
TEMPLATE = app
QT += network widgets

DEPENDPATH += . \
    headers \
    interfaces \
    tpl \
    src \
    ui \
    ../tools \
    ../plugins/pluginsmanager/ui \
    ../plugins/pluginsmanager/src \
    ../plugins/pluginsmanager/headers \

INCLUDEPATH += . \
    headers \
    interfaces \
    tpl \
    ../tools \
    ../plugins/pluginsmanager/headers \

# Inputs
RESOURCES += Images.qrc
HEADERS += tpl/Singleton.hpp
HEADERS += headers/QNetsoul.h \
    headers/Network.h \
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
    headers/Pastebin.h \
    headers/TrayIcon.h \
    headers/AbstractOptions.h \
    headers/OptionsWidget.h \
    headers/OptionsMainWidget.h \
    headers/OptionsContactsWidget.h \
    headers/OptionsChatWidget.h \
    headers/OptionsBlockedWidget.h \
    headers/OptionsFunWidget.h \
    headers/OptionsProxyWidget.h \
    headers/Popup.h \
    headers/SlidingPopup.h \
    headers/ContactsTree.h \
    headers/ToolTipBuilder.h \
    headers/InternUpdater.h \
    headers/Credentials.h \
    headers/CredentialsDialog.h

FORMS += ui/QNetsoul.ui \
    ui/Options.ui \
    ui/AddContact.ui \
    ui/Chat.ui \
    ui/CredentialsDialog.ui \
    ../plugins/pluginsmanager/ui/pluginsmanager.ui \

SOURCES += src/main.cpp \
    src/QNetsoul.cpp \
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
    src/TrayIcon.cpp \
    src/OptionsWidget.cpp \
    src/OptionsMainWidget.cpp \
    src/OptionsContactsWidget.cpp \
    src/OptionsChatWidget.cpp \
    src/OptionsBlockedWidget.cpp \
    src/OptionsFunWidget.cpp \
    src/OptionsProxyWidget.cpp \
    src/Popup.cpp \
    src/SlidingPopup.cpp \
    src/ContactsTree.cpp \
    src/ToolTipBuilder.cpp \
    src/InternUpdater.cpp \
    src/Credentials.cpp \
    src/CredentialsDialog.cpp

# Interfaces
HEADERS += interfaces/iplugindescriptor.h \
interfaces/ipopupplugin.h \
interfaces/iwidgetplugin.h \

# Plugins inputs
SOURCES += ../plugins/pluginsmanager/src/pluginsmanager.cpp
HEADERS += ../plugins/pluginsmanager/headers/pluginsmanager.h

# Common inputs
HEADERS += ../tools/tools.h
SOURCES += ../tools/tools.cpp

# Output
TARGET = QNetSoul
OBJECTS_DIR = obj
MOC_DIR = moc
UI_SOURCES_DIR = ui
UI_HEADERS_DIR = headers

unix:  DESTDIR = ../
win32: DESTDIR = ../
macx:  DESTDIR = .

win32: RC_FILE = appIcon/windows.rc
macx:  ICON = appIcon/qnetsoul.icns
