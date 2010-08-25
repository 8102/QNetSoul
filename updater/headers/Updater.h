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

#ifndef UPDATER_H_
#define UPDATER_H_

#include <QDir>
#include <QFile>
#include <QLocalSocket>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "ui_Updater.h"

class   QKeyEvent;

// TODO: manage the proxy/credentials !
class   Updater : public QWidget, public Ui_Updater
{
  Q_OBJECT

    public:
  Updater(QWidget* parent = NULL);
  ~Updater(void);

  public slots:
  void  checkVersion(void);

 protected:
  void  keyPressEvent(QKeyEvent* event);
  void  closeEvent(QCloseEvent* event);

 private:
  void  retrieveLastVersion(void);
  void  download(const QString& url, const QString& filename);
  void  install(void);
  bool  unzip(void);
  bool  replaceBinary(void);
  void  setupDownloadsDir(void);
  void  appendLog(const QString& msg);

  private slots:
  void  handleBytesReceived(void);
  void  handleFinishedRequest(QNetworkReply* reply);
  void  updateProgressBar(qint64 bytesReceived, qint64 bytesTotal);

 private:
  QFile*                _file;
  QNetworkReply*        _dlReply; // download reply
  QNetworkReply*        _lvReply; // last version reply
  QNetworkAccessManager _manager;
  QLocalSocket          _socket;
  QDir                  _downloadPath;
  QString               _url;
  QString               _version;
};

#endif
