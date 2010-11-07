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
#include <QTimer>
#include <QKeyEvent>
#include <QProcess>
#include <QLocalServer>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "QNetsoul.h"
#include "TrayIcon.h"
#include "Credentials.h"
#include "Singleton.hpp"
#include "InternUpdater.h"
#include "tools.h"

extern const QString OutDir;
extern const QString ServerUrl;
extern const QString LastVersionUrl;

namespace
{
#ifdef Q_OS_WIN
  const QString UpdaterBinaryName = "Updater.exe";
  const QString SevenZipBinaryName = "7za.exe";
#else
  const QString UpdaterBinaryName = "Updater";
  const QString SevenZipBinaryName = "7za.bin";
#endif
  const int OneHour = 3600000;
  const int ReplaceBinaryAttempts = 5;
}

InternUpdater::InternUpdater(QWidget* parent)
  : QObject(parent), _running(false), _checkVersionTimer(NULL),
    _trayIcon(NULL), _server(NULL), _sevenZipReply(NULL),
    _checkVersionReply(NULL), _netManager(NULL)
{
  setupNetworkAccessManager();
  download7zipIfNeeded();
  replaceUpdaterBinaryIfNeeded();
  this->_server = new QLocalServer(this);
  connect(this->_server, SIGNAL(newConnection()), SIGNAL(quitApplication()));
  if (!this->_server->listen("QNetSoul"))
    {
      if (QLocalServer::removeServer("QNetSoul"))
        {
#ifndef QT_NO_DEBUG
          qDebug() << "[InternUpdater::InternUpdater]"
                   << "QLocalServer removed and listened again.";
#endif
          this->_running = this->_server->listen("QNetSoul");
        }
    }
  else this->_running = true;
  // Periodic version checking
  if (this->_running == true)
    {
      this->_checkVersionTimer = new QTimer(this);
      connect(this->_checkVersionTimer, SIGNAL(timeout()),
              this, SLOT(checkLastVersion()));
      this->_checkVersionTimer->start(OneHour);
    }
#ifndef QT_NO_DEBUG
  qDebug() << "[InternUpdater::InternUpdater]"
           << (this->_running?
               "QLocalServer is running." : "QLocalServer is not running.");
#endif
}

InternUpdater::~InternUpdater(void)
{
}

bool    InternUpdater::download7zipIfNeeded(void)
{
  if (this->_sevenZipReply != NULL) return false; // already downloading
  QDir downloadPath(QDir::current());
  if (!downloadPath.exists(OutDir))
    downloadPath.mkdir(OutDir);
  downloadPath.cd(OutDir);
  if (downloadPath.exists(SevenZipBinaryName))
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[InternUpdater::download7zipIfNeeded]"
               << SevenZipBinaryName << "is already downloaded.";
#endif
      // Check if 7z has exec rights
      QFile bin(downloadPath.path() +QDir::separator()+ SevenZipBinaryName);
      if (bin.open(QIODevice::ReadOnly))
	{
	  bin.setPermissions(bin.permissions() | QFile::ExeOwner);
	  bin.close();
	}
      return false;
    }
  QUrl url(ServerUrl + SevenZipBinaryName);
  this->_sevenZipReply = this->_netManager->get(QNetworkRequest(url));
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
  args << Tools::qnetsoulVersion()
       << QDir::currentPath();
  QProcess::startDetached("./" + UpdaterBinaryName, args);
}

void    InternUpdater::checkLastVersion(void)
{
  if (this->_trayIcon == NULL)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[InternUpdater::checkLastVersion]"
               << "Unable to check: no tray icon.";
#endif
      return;
    }
  if (this->_checkVersionReply != NULL)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[InternUpdater::checkLastVersion]"
               << "Unable to check: already checking.";
#endif
      return;
    }
  const QString platform = Tools::identifyPlatform(QNS_RAW);
  if (platform.isEmpty())
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[InternUpdater::checkLastVersion]"
               << "Unable to check: unsupported platform.";
#endif
      return;
    }
  QUrl url(LastVersionUrl + platform);
  this->_checkVersionReply = this->_netManager->get(QNetworkRequest(url));
}

void    InternUpdater::setupNetworkAccessManager(void)
{
  this->_netManager = new QNetworkAccessManager(this);
  connect(this->_netManager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(handleReplies(QNetworkReply*)));
  Credentials& instance = Singleton<Credentials>::Instance();
  connect(this->_netManager,
          SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&,
                                             QAuthenticator*)),
          &instance,
          SLOT(handleCredentials(const QNetworkProxy&, QAuthenticator*)));
}

void    InternUpdater::handleSevenZipReply(void)
{
  QDir downloadPath(QDir::current());
  if (!downloadPath.exists(OutDir))
    downloadPath.mkdir(OutDir);
  downloadPath.cd(OutDir);
  QFile bin(downloadPath.path() + QDir::separator() + SevenZipBinaryName);
  if (bin.open(QIODevice::WriteOnly))
    {
      bin.write(this->_sevenZipReply->readAll());
      bin.setPermissions(QFile::ReadOwner  |
                         QFile::WriteOwner |
                         QFile::ExeOwner);
      bin.close();
#ifndef QT_NO_DEBUG
      qDebug() << "[InternUpdater::handleSevenZipReply]"
               << SevenZipBinaryName
               << "has been successfully downloaded.";
#endif
    }
#ifndef QT_NO_DEBUG
  else
    {
      qDebug() << "[InternUpdater::handleSevenZipReply]"
               << "Open failed:" << bin.errorString();
    }
#endif
}

void    InternUpdater::handleCheckVersionReply(void)
{
#ifndef QT_NO_DEBUG
  qDebug() << "[Updater::handleCheckVersionReply]"
           << "Last version retrieved";
#endif
  const QString buffer
    (QString::fromUtf8(this->_checkVersionReply->readAll()));
  if (buffer.startsWith("Error:"))
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[Updater::handleCheckVersionReply]"
               << buffer;
#endif
      return;
    }
  if (buffer.section(' ', 0, 0) > Tools::qnetsoulVersion())
    this->_trayIcon->showMessage(tr("Update available !"),
                                 "QNetSoul v" +
                                 buffer.section(' ', 0, 0) +
                                 tr(" is released."), 30000);
}

void    InternUpdater::handleReplies(QNetworkReply* reply)
{
  if (reply->error() == QNetworkReply::NoError)
    {
      if (reply == this->_sevenZipReply)
        {
          handleSevenZipReply();
          this->_sevenZipReply = NULL;
        }
      else if (reply == this->_checkVersionReply)
        {
          handleCheckVersionReply();
          this->_checkVersionReply = NULL;
        }
    }
#ifndef QT_NO_DEBUG
  else
    {
      qDebug() << "[InternUpdater::handleReplies]"
               << "Reply error string:" << reply->errorString();
      qDebug() << "Reply dump:" << reply;
    }
#endif
  reply->deleteLater();
}

void    InternUpdater::replaceUpdaterBinaryIfNeeded(void)
{
  static int attempts = 0;
  QDir downloadPath(QDir::current());

  if (!downloadPath.exists(OutDir)) return;
  downloadPath.cd(OutDir);
  if (!downloadPath.exists(UpdaterBinaryName)) return;
  QDir destPath(QDir::current());
  destPath.makeAbsolute();
  destPath.remove(UpdaterBinaryName);
  const bool moveResult =
    downloadPath.rename(downloadPath.filePath(UpdaterBinaryName),
                        destPath.path()+QDir::separator()+UpdaterBinaryName);
#ifndef QT_NO_DEBUG
  qDebug() << "[InternUpdater::replaceUpdaterBinaryIfNeeded]"
           << "moveResult:" << moveResult;
#endif
  if (moveResult == false && attempts++ < ReplaceBinaryAttempts)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[InternUpdater::replaceUpdaterBinaryIfNeeded]"
               << "replaceUpdaterBinaryIfNeeded called again in five seconds.";
      qDebug() << "Remaining attempts:" << ReplaceBinaryAttempts - attempts;
#endif
      QTimer::singleShot(5000, this, SLOT(replaceUpdaterBinaryIfNeeded()));
    }
}
