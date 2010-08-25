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
#include "QNetsoul.h"
#include "InternUpdater.h"

namespace
{
#ifdef Q_OS_WIN
  const QString BinaryName = "Updater.exe";
#else
  const QString BinaryName = "Updater";
#endif
  const QString dlDir = "downloads";
}

InternUpdater::InternUpdater(QWidget* parent) : QObject(parent)
{
  replaceUpdaterBinaryIfNeeded();
  this->_server = new QLocalServer(this);
  connect(this->_server, SIGNAL(newConnection()), SIGNAL(quitApplication()));
  if (!this->_server->listen("QNetSoul"))
    {
      if (QLocalServer::removeServer("QNetSoul"))
        this->_running = this->_server->listen("QNetSoul");
      return;
    }
#ifndef QT_NO_DEBUG
  qDebug() << "[InternUpdater::InternUpdater]"
           << "Server is running :)";
#endif
  this->_running = true;
}

InternUpdater::~InternUpdater(void)
{
}

void    InternUpdater::startUpdater(void)
{
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
  // TODO: find out Windows' way
  QProcess::startDetached("./Updater", args);
}

bool    InternUpdater::replaceUpdaterBinaryIfNeeded(void)
{
  QDir downloadPath(QDir::current());
  if (!downloadPath.exists(dlDir)) return false;
  downloadPath.cd(dlDir);
  if (!downloadPath.exists(BinaryName)) return false;
  QDir destPath(QDir::current());
  destPath.makeAbsolute();
  destPath.remove(BinaryName);
  const bool moveResult =
    downloadPath.rename(downloadPath.filePath(BinaryName),
			destPath.path() + QDir::separator() + BinaryName);
#ifndef QT_NO_DEBUG
  qDebug() << "[InternUpdater::replaceUpdaterBinaryIfNeeded]"
           << "moveResult:" << moveResult;
#endif
  return moveResult;
}
