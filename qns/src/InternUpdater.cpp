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

#include <QDir>
#include <QDebug>
#include <QKeyEvent>
#include <QProcess>
#include <QLocalServer>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "QNetsoul.h"
#include "Credentials.h"
#include "Singleton.hpp"
#include "InternUpdater.h"

namespace
{
#ifdef Q_OS_WIN
  const QString UpdaterBinaryName = "Updater.exe";
  const QString SevenZipBinaryName = "7za.exe";
#else
  const QString UpdaterBinaryName = "Updater";
  const QString SevenZipBinaryName = "7za.bin";
#endif
  const QString dlDir = "downloads";
}

InternUpdater::InternUpdater(QWidget* parent)
  : QObject(parent), _running(false)
{
  setupNetworkAccessManager();
  download7zipIfNeeded();
  replaceUpdaterBinaryIfNeeded();
  this->_server = new QLocalServer(this);
  connect(this->_server, SIGNAL(newConnection()), SIGNAL(quitApplication()));
  if (!this->_server->listen("QNetSoul"))
    {
      if (QLocalServer::removeServer("QNetSoul"))
        this->_running = this->_server->listen("QNetSoul");
    }
  else this->_running = true;
#ifndef QT_NO_DEBUG
  qDebug() << "[InternUpdater::InternUpdater]"
           << (this->_running?
	       "LocalServer is running." : "LocalServer is not running.");
#endif
}

InternUpdater::~InternUpdater(void)
{
}

bool    InternUpdater::download7zipIfNeeded(void)
{
  QDir downloadPath(QDir::current());
  if (!downloadPath.exists(dlDir))
    downloadPath.mkdir(dlDir);
  downloadPath.cd(dlDir);
  if (downloadPath.exists(SevenZipBinaryName))
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[InternUpdater::download7zipIfNeeded]"
               << SevenZipBinaryName << "is already downloaded.";
#endif
      return false;
    }
  QUrl url("http://qnetsoul.tuxfamily.org/public/" + SevenZipBinaryName);
  this->_netManager->get(QNetworkRequest(url));
#ifndef QT_NO_DEBUG
  qDebug() << "[InternUpdater::download7zipIfNeeded]"
           << "Downloading" << SevenZipBinaryName;
#endif
  return true;
}

void    InternUpdater::startUpdater(void)
{
  download7zipIfNeeded();
  if (this->_running == false)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[InternUpdater::startUpdater]"
               << "Server is not running so we do not start Updater";
#endif
      return;
    }
  QStringList args;
  args << QNetsoul::currentVersion()
       << QDir::currentPath();
  QProcess::startDetached("./" + UpdaterBinaryName, args);
}

void    InternUpdater::setupNetworkAccessManager(void)
{
  this->_netManager = new QNetworkAccessManager(this);
  connect(this->_netManager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(finishedDownload(QNetworkReply*)));
  Credentials& instance = Singleton<Credentials>::Instance();
  connect(this->_netManager,
	  SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&,
					     QAuthenticator*)),
	  &instance,
	  SLOT(handleCredentials(const QNetworkProxy&, QAuthenticator*)));
}

bool    InternUpdater::replaceUpdaterBinaryIfNeeded(void)
{
  QDir downloadPath(QDir::current());
  if (!downloadPath.exists(dlDir)) return false;
  downloadPath.cd(dlDir);
  if (!downloadPath.exists(UpdaterBinaryName)) return false;
  QDir destPath(QDir::current());
  destPath.makeAbsolute();
  //if (!destPath.rename(UpdaterBinaryName, UpdaterBinaryName + ".old"))
  destPath.remove(UpdaterBinaryName);
  const bool moveResult =
    downloadPath.rename(downloadPath.filePath(UpdaterBinaryName),
                        destPath.path()+QDir::separator()+UpdaterBinaryName);
#ifndef QT_NO_DEBUG
  qDebug() << "[InternUpdater::replaceUpdaterBinaryIfNeeded]"
           << "moveResult:" << moveResult;
#endif
  return moveResult;
}

void    InternUpdater::finishedDownload(QNetworkReply* reply)
{
  if (reply->error() == QNetworkReply::NoError)
    {
      QDir downloadPath(QDir::current());
      if (!downloadPath.exists(dlDir))
        downloadPath.mkdir(dlDir);
      downloadPath.cd(dlDir);
      QFile bin(downloadPath.path()+QDir::separator() + SevenZipBinaryName);
      if (bin.open(QIODevice::WriteOnly))
        {
          bin.write(reply->readAll());
          bin.setPermissions(QFile::ReadOwner  |
                             QFile::WriteOwner |
                             QFile::ExeOwner);
          bin.close();
#ifndef QT_NO_DEBUG
          qDebug() << "[InternUpdater::finishedDownload]"
                   << SevenZipBinaryName
                   << "has been successfully downloaded.";
#endif
        }
#ifndef QT_NO_DEBUG
      else
        {
          qDebug() << "[InternUpdater::finishedDownload]"
                   << "Open failed:" << bin.error();
        }
#endif
    }
#ifndef QT_NO_DEBUG
  else
    {
      qDebug() << "[InternUpdater::finishedDownload]"
               << "Reply error:" << reply->error();
    }
#endif
  reply->deleteLater();
}
