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

#ifndef PORTRAIT_RESOLVER_H_
#define PORTRAIT_RESOLVER_H_

#include <QDir>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#define QNS_NORMAL false
#define QNS_FUN    true

class   TrayIcon;

class   PortraitResolver : public QNetworkAccessManager
{
  Q_OBJECT

  public:
  PortraitResolver(void);
  ~PortraitResolver(void);

  void  setTrayIcon(TrayIcon* ti) { this->_trayIcon = ti; }

  void  addRequest(const QStringList& logins);

  static bool isAvailable(QString& portraitsPath,
                          const QString& login,
                          bool fun = QNS_NORMAL);
  static QString buildFilename(const QString& login, const bool fun);
  static QDir getPortraitDir(void);

public slots:
  void  addRequest(const QString& login, bool fun);

private slots:
  void  replyFinished(QNetworkReply* reply);

signals:
  void  downloadedPortrait(const QString& login);

private:
  void  setupPortraitDirectory(void);

private:
  QDir      _dir;
  TrayIcon* _trayIcon;
};

#endif // PORTRAITRESOLVER_H_
