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

#ifndef INTERN_UPDATER_H_
#define INTERN_UPDATER_H_

#include <QObject>

class   QTimer;
class   TrayIcon;
class   QProcess;
class   QLocalServer;
class   QNetworkReply;
class   QNetworkAccessManager;

class   InternUpdater : public QObject
{
  Q_OBJECT

  public:
  InternUpdater(QWidget* parent);
  ~InternUpdater(void);

  void  setTrayIcon(TrayIcon* trayIcon) { this->_trayIcon = trayIcon; }
  bool  download7zipIfNeeded(void);

public slots:
  void  startUpdater(void);
  void  checkLastVersion(void);

signals:
  void  quitApplication(void);

private:
  void  setupNetworkAccessManager(void);
  void  handleSevenZipReply(void);
  void  handleCheckVersionReply(void);

private slots:
  void  handleReplies(QNetworkReply* reply);
  void  replaceUpdaterBinaryIfNeeded(void);

private:
  bool                   _running;
  QTimer*                _checkVersionTimer;
  TrayIcon*              _trayIcon;
  QLocalServer*          _server;
  QNetworkReply*         _sevenZipReply;
  QNetworkReply*         _checkVersionReply;
  QNetworkAccessManager* _netManager;
};

#endif
