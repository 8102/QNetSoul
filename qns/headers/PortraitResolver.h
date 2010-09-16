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

class   PortraitResolver : public QNetworkAccessManager
{
  Q_OBJECT

    public:
  PortraitResolver(void);
  ~PortraitResolver(void);

  void  addRequest(const QStringList& logins);
  void  addRequest(const QString& login, bool fun);

  static bool isAvailable(QString& path, const QString& login);
  static QString buildFilename(const QString& login, const bool fun);
  static QDir getPortraitDir(void);

  public slots:
  void	addRequest(const QString& login);

  private slots:
  void  replyFinished(QNetworkReply* reply);

 signals:
  void  downloadedPortrait(const QString& login);

 private:
  enum Attribute { Login = QNetworkRequest::User, Fun };
  void  setupPortraitDirectory(void);

 private:
  QDir _dir;
};

#endif // PORTRAITRESOLVER_H_
