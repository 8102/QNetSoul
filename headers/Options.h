/*
  Copyright 2009 Dally Richard
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include <QNetworkProxy>
#include "ui_Options.h"

class Options : public QDialog, public Ui_Options
{
  Q_OBJECT

    public:
  Options(QWidget* parent);
  virtual ~Options(void);

  void	readOptionSettings(void);
  void	writeOptionSettings(void);
  void	updateOptions(void);
  bool	startingVdm(void) const { return this->_startingVdm; }
  bool	autoConnect(void) const { return this->_autoConnect; }
  void	setConnectionOnOk(const bool& value) { this->_connectOnOk = value; }
  bool	isProxyEnabled(void) const { return this->_useProxy; }
  bool	isValidProxy(void) const;
  const QNetworkProxy getProxy(void) const;
  void	applyOptions(void);
  QString getReply(int index);

signals:
  void	loginPasswordFilled(void);
  void	resetProxy(const QNetworkProxy& p = QNetworkProxy());
  void	chatOptionsChanged(bool, bool, bool);

private slots:
  void	save(void);
  void	loadReply(int index);

private:
  void	saveCurrentReply(void)
  {
    saveCurrentReply(this->autoReplyComboBox->currentIndex());
  }
  void	saveCurrentReply(int index);
  void	setCheckState(QCheckBox* checkBox, const bool state)
  {
    (state)?
      checkBox->setCheckState(Qt::Checked) :
      checkBox->setCheckState(Qt::Unchecked);
  }

  // Main Tab
  QString	_server;
  QString	_port;
  QString	_login;
  QString	_location;
  QString	_comment;
  QString	_password;
  bool		_savePassword;
  bool		_autoConnect;
  bool		_connectOnOk;
  // Chat tab
  bool		_exitOnEscape;
  bool		_typingNotification;
  bool		_smileys;
  int		_oldComboBoxValue;
  int		_replyComboBoxValue;
  QString	_replyLocked;
  QString	_replyAway;
  QString	_replyServer;
  // Fun tab
  bool		_startingVdm;
  // Advanced Tab
  bool		_useProxy;
  QString	_proxy;
  QString	_proxyPort;
  QString	_proxyLogin;
  QString	_proxyPassword;
};

#endif // OPTIONS_H
