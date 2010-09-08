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

#include <QDebug>
#include <QProcess>
#include <QKeyEvent>
#include <QMessageBox>
#include <QNetworkReply>
#include "Updater.h"
#include "Credentials.h"
#include "Singleton.hpp"

/*
 * 1) Retrieve last current version for the platform
 * 2) If current version < last version then download it
 *    Else exit updater
 * 3) Close main application if download succeed AND checksum is valid
 * 4) Replace old binary by downloaded binary
 * 5) Relaunch main application
 */

namespace
{
#ifdef Q_OS_WIN
  const QString BinaryName = "QNetSoul.exe";
  const QString SevenZipBinaryName = "7za.exe";
#else
  const QString BinaryName = "QNetSoul";
  const QString SevenZipBinaryName = "7za.bin";
#endif
  const QString dlDir = "downloads";
  const QString ftp = "http://qnetsoul.tuxfamily.org/public/";
  const QString lastVersionUrl = ftp + "GetLastVersion?platform=";
}

Updater::Updater(QWidget* parent)
  : QWidget(parent), _file(NULL), _dlReply(NULL), _lvReply(NULL),
    _7zReply(NULL), _downloadPath(QDir::current())
{
  setupUi(this);
  move(400, 300);
  resize(100, 100);
  this->progressBar->hide();
  this->textBrowser->hide();
  Credentials& instance = Singleton<Credentials>::Instance();
  connect(this->pushButton, SIGNAL(clicked()), SLOT(checkVersion()));
  connect(&this->_manager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(handleFinishedRequest(QNetworkReply*)));
  connect(&this->_manager,
          SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&,
                                             QAuthenticator*)),
          &instance,
          SLOT(handleCredentials(const QNetworkProxy&, QAuthenticator*)));
}

Updater::~Updater(void)
{
  if (this->_file)
    {
      if (this->_file->isOpen())
        this->_file->close();
      delete this->_file;
    }
}

void    Updater::checkVersion(void)
{
  this->progressBar->show();
  this->progressBar->reset();
  this->textBrowser->show();
  this->pushButton->setEnabled(false);
  retrieveLastVersion();
}

void    Updater::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape)
    qApp->quit();
}

void    Updater::closeEvent(QCloseEvent* event)
{
#ifndef QT_NO_DEBUG
  qDebug() << "[Updater::closeEvent]";
#endif
  QWidget::closeEvent(event);
}

void    Updater::retrieveLastVersion(void)
{
  QString platform;

#if defined(Q_OS_WIN)
  platform = "Win";
#elif defined(Q_OS_LINUX)
  platform = "Linux";
#elif defined(Q_OS_MAC)
  platform = "Mac";
#else
  appendLog(tr("Unknown platform."));
  return;
#endif

  // Platform version
  platform += "32";

#if defined (QT_SHARED) || defined (QT_DLL)
  platform += "shared";
#else
  platform += "static";
#endif

  appendLog(tr("Detected platform: ") + platform);
  appendLog(tr("Retrieving last version on TuxFamily..."));
  QUrl url(lastVersionUrl + platform);
  this->_lvReply = this->_manager.get(QNetworkRequest(url));
  connect(this->_lvReply, SIGNAL(downloadProgress(qint64, qint64)),
          SLOT(updateProgressBar(qint64, qint64)));
}

void    Updater::download(const QString& url, const QString& filename)
{
  Q_ASSERT(this->_file == NULL);
  Q_ASSERT(!filename.isEmpty());
  this->_file = new QFile(dlDir + QDir::separator() + filename);
  if (!this->_file->open(QIODevice::WriteOnly))
    {
#ifndef QT_NO_DEBUG
      qDebug() << "Opening file failed";
#endif
      delete this->_file;
      this->_file = NULL;
      return;
    }
  this->_dlReply = this->_manager.get(QNetworkRequest(QUrl(url)));
  connect(this->_dlReply, SIGNAL(downloadProgress(qint64, qint64)),
          this, SLOT(updateProgressBar(qint64, qint64)));
  connect(this->_dlReply, SIGNAL(readyRead()), SLOT(handleBytesReceived()));
}

void    Updater::install(void)
{
  appendLog(tr("Installing last version of QNetSoul"));
  if (unzip() == false) return;
  if (this->_downloadPath.exists(BinaryName) == false)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[Updater::install]" << BinaryName + " cannot be found...";
#endif
      return;
    }
  QMessageBox::information(this, "QNetSoul Updater",
                           tr("QNetSoul is about to restart") + "<br />" +
                           tr("When you are ready press Ok"));
  // Shutting down QNetSoul to replace binary
  this->_socket.connectToServer("QNetSoul");
  this->_socket.waitForDisconnected(3000);
  if (replaceBinary() == false) return;
  if (QProcess::startDetached("./" + BinaryName))
    appendLog(tr("QNetSoul has been successfully updated :)"));
}

// 7zip powered !
bool    Updater::unzip(void)
{
  QProcess* unzip = new QProcess(this);
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  unzip->setProcessEnvironment(env);
  unzip->setWorkingDirectory(this->_downloadPath.path());
  QStringList args;
  args << "e"
       << this->_downloadPath.path() + QDir::separator() + "out";
  unzip->start(this->_downloadPath.path() + QDir::separator() +
               SevenZipBinaryName, args);
  const bool result = unzip->waitForFinished(3000);
  delete unzip;
#ifndef QT_NO_DEBUG
  qDebug() << "[Updater::unzip]" << "7zip result:" << result;
#endif
  return result;
}

bool    Updater::replaceBinary(void)
{
  QString destPath;
  QDir dest(this->_downloadPath);
  dest.cdUp();
  dest.makeAbsolute();
  destPath = dest.path() + QDir::separator() + BinaryName;
  //if (this->_downloadPath.rename(destPath, destPath + ".old") == false)
  dest.remove(BinaryName); // removing QNetSoul
  const bool moveResult =
    this->_downloadPath
    .rename(this->_downloadPath.filePath(BinaryName), destPath);
#ifndef QT_NO_DEBUG
  qDebug() << "[Updater::replaceBinary]" << "moveResult:" << moveResult;
#endif
  return moveResult;
}

void    Updater::setupDownloadsDir(void)
{
  if (!this->_downloadPath.exists(dlDir))
    this->_downloadPath.mkdir(dlDir);
  this->_downloadPath.cd(dlDir);
  this->_downloadPath.makeAbsolute();
}

void    Updater::appendLog(const QString& msg)
{
  this->textBrowser->insertHtml("<i>" + msg + "</i><br />");
}

void    Updater::handleBytesReceived(void)
{
  Q_ASSERT(this->_file && this->_dlReply);
  this->_file->write(this->_dlReply->readAll());
}

void    Updater::handleFinishedRequest(QNetworkReply* reply)
{
  if (reply == this->_lvReply)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[Updater::handleFinishedRequest]"
               << "Last version retrieved";
#endif
      if (reply->error() == QNetworkReply::NoError)
        {
          const QString buffer(QString::fromUtf8(reply->readAll()));
          if (buffer.startsWith("Error:"))
            {
              appendLog(buffer);
            }
          else if (buffer.section(' ', 0, 0) >
		   QCoreApplication::arguments().at(1))
            {
              this->label->setText(tr("A new version is available !"));
              QString msg = QString(tr("Downloading QNetSoul v%1 on TuxFamily"))
                .arg(buffer.section(' ', 0, 0));
              appendLog(msg);
              setupDownloadsDir();
              download(ftp + buffer.section(' ', -1), "out");
            }
          else
            {
              this->label->setText(tr("Your version is up-to-date."));
              appendLog(tr("You got the last version for your platform: v")
                        + QCoreApplication::arguments().at(1));
            }
        }
      else
        {
          appendLog(reply->errorString());
        }
      this->_lvReply = NULL;
    }
  else if (reply == this->_dlReply)
    {
      Q_ASSERT(this->_file);
#ifndef QT_NO_DEBUG
      qDebug() << "Download is now finished";
#endif
      this->_file->close();
      this->_dlReply = NULL;
      if (reply->error() == QNetworkReply::NoError)
        {
          install();
        }
      else
        {
          this->_file->remove();
          appendLog(reply->errorString());
        }
    }
  reply->deleteLater();
}

void    Updater::updateProgressBar(qint64 bytesReceived, qint64 bytesTotal)
{
  this->progressBar->setMaximum(bytesTotal);
  this->progressBar->setValue(bytesReceived);
}
