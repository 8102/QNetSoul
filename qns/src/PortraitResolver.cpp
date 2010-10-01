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

#include <QImage>
#include <QMessageBox>
#include <QNetworkProxy>
#include "Url.h"
#include "PortraitResolver.h"

namespace
{
  const QString DirName = "portraits";
}

PortraitResolver::PortraitResolver(void)
  : _dir(getPortraitDir())
{
  setProxy(QNetworkProxy::NoProxy);
  connect(this, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(replyFinished(QNetworkReply*)));
}

PortraitResolver::~PortraitResolver(void)
{
}

void    PortraitResolver::addRequest(const QStringList& logins)
{
  const int size = logins.size();
  for (int i = 0; i < size; ++i)
    addRequest(logins[i], false);
}

void    PortraitResolver::addRequest(const QString& login, bool fun)
{
  if (this->_dir.exists(buildFilename(login, fun))) return;
  QUrl url(QString("http://www.epitech.net/intra/photo.php?fun=%1&login=%2")
           .arg(fun).arg(login));
  get(QNetworkRequest(url));
}

bool    PortraitResolver::isAvailable(QString& portraitPath,
                                      const QString& login)
{
  const QDir portraitDir = getPortraitDir();
  const QString fileName = buildFilename(login, false);
  if (portraitDir.exists(fileName))
    {
      portraitPath = portraitDir.dirName() + QDir::separator() + fileName;
      return true;
    }
  return false;
}

QString PortraitResolver::buildFilename(const QString& login, const bool fun)
{
  // WARNING: test this extension instead jpeg
  return fun? (login + "1.png") : (login + "0.png");
}

QDir    PortraitResolver::getPortraitDir(void)
{
  QDir portraitPath(QDir::currentPath());
  if (!portraitPath.exists(DirName))
    portraitPath.mkdir(DirName);
  portraitPath.cd(DirName);
  return portraitPath;
}

void    PortraitResolver::addRequest(const QString& login)
{
  addRequest(login, true);
  addRequest(login, false);
}

void    PortraitResolver::replyFinished(QNetworkReply* reply)
{
  if (reply->error() != QNetworkReply::NoError)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[PortraitResolver::replyFinished]"
               << "Reply failed, reason:"
               << reply->error();
#endif
      reply->deleteLater();
      return;
    }
  const bool fun = reply->url().toString()
    .section('=', 1, 1).section('&', 0, 0).contains("1");
  const QString login = reply->url().toString().section('=', -1);

  QImage img = QImage::fromData(reply->readAll());
  if (img.isNull())
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[PortraitResolver::replyFinished]"
	       << "img is NULL, website is probably down.";
#endif
    }
  else if (img.save(DirName + QDir::separator() + buildFilename(login, fun)))
    {
      if (fun == false)
        emit downloadedPortrait(login);
    }
#ifndef QT_NO_DEBUG
  else
    {
      qDebug() << "[PortraitResolver::replyFinished]"
               << "QImage saving failed with"
               << login;
    }
#endif
  reply->deleteLater();
}

void    PortraitResolver::setupPortraitDirectory(void)
{
  if (!this->_dir.exists(DirName))
    this->_dir.mkdir(DirName);
  this->_dir.cd(DirName);
}
