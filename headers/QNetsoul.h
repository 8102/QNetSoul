/*
  Copyright 2010 Dally Richard
  This file is part of QNetSoul.
  QNetSoul is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  QNetSoul is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with QNetSoul.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QNETSOUL_H_
#define QNETSOUL_H_

#include <QtGui>
#include <QTimer>
#include <QCryptographicHash>
#include "Chat.h"
#include "Options.h"
#include "Network.h"
#include "AddContact.h"
#include "PortraitResolver.h"
#include "Pastebin.h"
#include "VieDeMerde.h"
#include "ChuckNorrisFacts.h"
#include "QSlidingPopup.h"
#include "TrayIcon.h"
#include "ui_QNetsoul.h"

class   QMenu;
class   QAction;
class   QLabel;
class   QSystemTrayIcon;
class   ContactWidget;
class   TrayIcon;

class   QNetsoul : public QMainWindow, public Ui_QNetsoul
{
  Q_OBJECT

    public:
  QNetsoul(QWidget* parent = 0);
  ~QNetsoul(void);

  static QString currentVersion(void)
  {
    return "0.6";
  }
  static QString defaultComment(void)
  {
    return QString("-=[QNetSoul v%1]=-").arg(currentVersion());
  }

 protected:
  void  closeEvent(QCloseEvent*);

  private slots:
  void  connectToServer(void);
  void  ping(void);
  void  reconnect(void);
  void  disconnect(void);
  void  updateWidgets(const QAbstractSocket::SocketState&);
  void	checkForUpdates(void);

  void  saveStateBeforeQuiting(void);
  void  openOptionsDialog(QLineEdit* newLineFocus = 0);
  void  handleClicksOnTrayIcon(QSystemTrayIcon::ActivationReason);
  void  refreshContacts(void);
  void  sendStatus(const int&) const;
  void  changeStatus(const QStringList& properties);
  void  updateContact(const QStringList& properties);
  void  showConversation(const QStringList&, const QString& msg = "");
  void  processHandShaking(int, QStringList);
  void  transmitMsg(const int id, const QString& msg);
  void  transmitTypingStatus(const QString&, bool);
  void  notifyTypingStatus(const int id, const bool typing);
  void  setPortrait(const QString&);
  void  aboutQNetSoul(void);

 private:
  Chat* getChat(const int id);
  void  watchLogContacts(void);
  void  watchLogContact(const QString&);
  void  refreshContact(const QString&);
  void  resetAllContacts(void);
  void  readSettings(void);
  void  writeSettings(void);
  void  connectQNetsoulItems(void);
  void  connectActionsSignals(void);
  void  connectNetworkSignals(void);
  Chat* createWindowChat(const int, const QString&, const QString&);
  void  deleteAllWindowChats(void);

  Network*          _network;
  Options*          _options;
  TrayIcon*         _trayIcon;
  QHash<int, Chat*> _windowsChat;
  QString           _timeStamp;
  QPoint            _oldPos;
  PortraitResolver  _portraitResolver;
  Pastebin          _pastebin;
  QSlidingPopup     _popup;
  VieDeMerde        _vdm;
  ChuckNorrisFacts  _cnf;
  QTimer            _ping;
};

#endif // QNETSOUL_H_
