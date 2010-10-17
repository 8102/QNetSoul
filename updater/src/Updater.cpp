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
#include <QTimer>
#include <QProcess>
#include <QKeyEvent>
#include <QSettings>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkProxy>
#include "Updater.h"
#include "Credentials.h"
#include "Singleton.hpp"
#include "tools.h"

/*
 * 1) Retrieve last current version for the platform
 * 2) If current version < last version then download it
 *    Else exit updater
 * 3) Close main application if download succeed AND checksum is valid (TODO)
 * 4) Replace old binary by downloaded binary
 * 5) Relaunch main application
 */

extern const QString OutDir;
extern const QString ServerUrl;
extern const QString ServerName;
extern const QString LastVersionUrl;

namespace
{
#ifdef Q_OS_WIN
  const QString QNetSoulBinaryName = "QNetSoul.exe";
  const QString SevenZipBinaryName = "7za.exe";
#else
  const QString QNetSoulBinaryName = "QNetSoul";
  const QString SevenZipBinaryName = "7za.bin";
#endif
}

Updater::Updater(void)
  : _file(NULL), _dlReply(NULL), _lvReply(NULL),
    _7zReply(NULL), _downloadPath(QDir::current())
{
  setupUi(this);
  move(400, 300);
  resize(100, 100);
  initProxyFromSettings();
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

void    Updater::initProxyFromSettings(void)
{
  QSettings settings("Epitech", "QNetsoul");

  settings.beginGroup("AdvancedOptions");
  if (settings.value("useproxy", bool(false)).toBool() == true)
    {
      const QString proxyServer =
        settings.value("proxy", QString("proxy.epitech.net")).toString();
      const QString proxyPort =
        settings.value("port", QString("3128")).toString();
      const QString proxyLogin = settings.value("login").toString();
      QString proxyPassword = settings.value("password").toString();

      bool port_ok;
      proxyPort.toUShort(&port_ok);
      if (!proxyServer.isEmpty() && !proxyLogin.isEmpty() &&
          !proxyPassword.isEmpty() && port_ok)
        {
          proxyPassword = Tools::unencrypt(proxyPassword);
          QNetworkProxy proxy(QNetworkProxy::HttpProxy,
                              proxyServer,
                              proxyPort.toUShort(),
                              proxyLogin,
                              proxyPassword);
          QNetworkProxy::setApplicationProxy(proxy);
#ifndef QT_NO_DEBUG
          qDebug() << "[Updater::initProxyFromSettings]"
                   << "Proxy has been set.";
#endif
        }
    }
  settings.endGroup();
}

void    Updater::retrieveLastVersion(void)
{
  const QString platform = Tools::identifyPlatform(QNS_RAW);
  if (platform.isEmpty())
    {
      appendLog(tr("Unsupported platform."));
      return;
    }
  appendLog(tr("Detected platform: ") + platform);
  appendLog(tr("Retrieving last version on ") + ServerName);
  QUrl url(LastVersionUrl + platform);
  this->_lvReply = this->_manager.get(QNetworkRequest(url));
  connect(this->_lvReply, SIGNAL(downloadProgress(qint64, qint64)),
          SLOT(updateProgressBar(qint64, qint64)));
}

void    Updater::download(const QString& url, const QString& filename)
{
  Q_ASSERT(this->_file == NULL);
  Q_ASSERT(!filename.isEmpty());
  this->_file = new QFile(OutDir + QDir::separator() + filename);
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
  if (this->_downloadPath.exists(QNetSoulBinaryName) == false)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[Updater::install]" << QNetSoulBinaryName + " cannot be found...";
#endif
      return;
    }
  QMessageBox::information(this, "QNetSoul Updater",
                           tr("QNetSoul is about to restart.") + "<br />" +
                           tr("When you are ready, press Ok."));
  // Shutting down QNetSoul to replace binary
  this->_socket.connectToServer("QNetSoul");
  this->_socket.waitForDisconnected(3000);
  appendLog(tr("QNetSoul binary is about to be replaced in few seconds...."));
  QTimer::singleShot(2000, this, SLOT(replaceQNetSoulBinary()));
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

void    Updater::replaceQNetSoulBinary(void)
{
  QString destPath;
  QDir dest(this->_downloadPath);
  dest.cdUp();
  dest.makeAbsolute();
  destPath = dest.path() + QDir::separator() + QNetSoulBinaryName;
  dest.remove(QNetSoulBinaryName); // removing QNetSoul
  const bool moveResult =
    this->_downloadPath
    .rename(this->_downloadPath.filePath(QNetSoulBinaryName), destPath);
#ifndef QT_NO_DEBUG
  qDebug() << "[Updater::replaceBinary]" << "moveResult:" << moveResult;
#endif
  if (moveResult == false)
    {
      this->label->setText(tr("Last version is not installed yet"));
      appendLog(tr("Failure: ") + QNetSoulBinaryName + tr(" is still running."));
      appendLog(tr("Solution: replace binaries by hand."));
      return;
    }
  if (QProcess::startDetached("./" + QNetSoulBinaryName))
    {
      this->label->setText(tr("Last version successfully installed !"));
      appendLog(tr("QNetSoul has been successfully updated :)"));
      appendLog(tr("NB: Updater binary will be replaced when you'll close this window."));
    }
}

void    Updater::setupDownloadsDir(void)
{
  if (!this->_downloadPath.exists(OutDir))
    this->_downloadPath.mkdir(OutDir);
  this->_downloadPath.cd(OutDir);
  this->_downloadPath.makeAbsolute();
}

void    Updater::appendLog(const QString& msg)
{
  this->textBrowser->insertHtml(msg + "<br />");
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
              const QString msg = QString(tr("Downloading QNetSoul v%1 on TuxFamily"))
                .arg(buffer.section(' ', 0, 0));
              appendLog(msg);
              setupDownloadsDir();
              download(ServerUrl + buffer.section(' ', -1), "out");
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
      qDebug() << "[Updater::handleFinishedRequest]"
               << "Download is now finished";
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
