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
#include <QApplication>
#include <QNetworkProxy>
#include "Url.h"
#include "TrayIcon.h"
#include "PortraitResolver.h"

namespace
{
  const QString DirName = "portraits";
  const QString HttpServer = "http://qnetsoul.tuxfamily.org/public/";
  const QString UrlBase = "http://www.epitech.eu/intra/photos/";
}

PortraitResolver::PortraitResolver(void)
  : _dir(getPortraitDir()), _trayIcon(NULL)
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
    {
      addRequest(logins[i], QNS_FUN);
      addRequest(logins[i], QNS_NORMAL);
    }
}

bool    PortraitResolver::isAvailable(QString& portraitPath,
                                      const QString& login,
                                      bool fun)
{
  const QDir portraitDir = getPortraitDir();
  const QString fileName = buildFilename(login, fun);
  if (portraitDir.exists(fileName))
    {
      portraitPath = portraitDir.dirName() + QDir::separator() + fileName;
      return true;
    }
  return false;
}

QString PortraitResolver::buildFilename(const QString& login, const bool fun)
{
  return fun? (login + "1.jpg") : (login + "0.jpg");
}

QDir    PortraitResolver::getPortraitDir(void)
{
  QDir portraitPath(QDir::currentPath());
  if (!portraitPath.exists(DirName))
    portraitPath.mkdir(DirName);
  portraitPath.cd(DirName);
  return portraitPath;
}

void    PortraitResolver::addRequest(const QString& login, bool fun)
{
  if (fun == true) return; // temporary disable fun requests
  if (this->_dir.exists(buildFilename(login, fun))) return;
  QUrl url(UrlBase + login + ".jpg");
  get(QNetworkRequest(url));
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
  const bool fun = false;
  const QString login(reply->url().toString()
		      .section('.', -2, -2).section('/', -1));
  const QString fileName
    (QDir::toNativeSeparators(qApp->applicationDirPath()) + QDir::separator()
     + DirName + QDir::separator() + buildFilename(login, fun));

  QImage img = QImage::fromData(reply->readAll());
  if (img.isNull())
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[PortraitResolver::replyFinished]"
               << "img is NULL, website is probably down.";
#endif
    }
  else if (img.save(fileName, "JPEG"))
    {
      if (fun == false)
        emit downloadedPortrait(login);
    }
  else if (this->_trayIcon != NULL)
    {
      this->_trayIcon->showMessage
        (tr("Missing plugin") + " (jpeg)",
         tr("It seems you did not install Qt4 jpeg plugin.") + "\n" +
         tr("You can download it here:") + " " + HttpServer);
    }
  reply->deleteLater();
}

void    PortraitResolver::setupPortraitDirectory(void)
{
  if (!this->_dir.exists(DirName))
    this->_dir.mkdir(DirName);
  this->_dir.cd(DirName);
}
