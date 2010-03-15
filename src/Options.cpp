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

#include <iostream>
#include <QSettings>
#include "Options.h"
#include "Encryption.h"

Options::Options(QWidget* parent)
  : QDialog(parent), _connectOnOk(false), _oldComboBoxValue(-42)
{
  setupUi(this);
  readOptionSettings();
  update();
  connect(this, SIGNAL(accepted()), SLOT(save()));
  connect(this->autoReplyComboBox, SIGNAL(currentIndexChanged(int)), SLOT(loadReply(int)));
}

Options::~Options(void)
{
}

void		Options::readOptionSettings(void)
{
  QSettings	settings("Epitech", "QNetsoul");

  settings.beginGroup("MainOptions");
  this->_server = settings.value("ip", QString("ns-server.epita.fr")).toString();
  this->_port = settings.value("port", QString("4242")).toString();
  this->_login = settings.value("login", QString("")).toString();
  this->_location = settings.value("location", QString("%L")).toString();
  this->_comment = settings.value("comment", QString("-=[QNetSoul]=-")).toString();
  this->_password = settings.value("password", QString("")).toString();
  this->_savePassword = settings.value("savepassword", bool(false)).toBool();
  settings.endGroup();

  settings.beginGroup("ChatOptions");
  this->_exitOnEscape = settings.value("exitonescape", bool(false)).toBool();
  this->_typingNotification = settings.value("typingnotification", bool(false)).toBool();
  this->_smileys = settings.value("smileys", bool(false)).toBool();
  this->_replyComboBoxValue = settings.value("replycomboboxvalue", int(0)).toInt();
  this->_replyLocked = settings.value("replylocked", QString("[Locked]")).toString();
  this->_replyAway = settings.value("replyaway", QString("[Away]")).toString();
  this->_replyServer = settings.value("replyserver", QString("[Daemonized]")).toString();
  settings.endGroup();

  settings.beginGroup("AdvancedOptions");
  this->_useProxy = settings.value("useproxy", bool(false)).toBool();
  this->_proxy = settings.value("proxy", QString("proxy.epitech.net")).toString();
  this->_proxyPort = settings.value("port", QString("3128")).toString();
  this->_proxyLogin = settings.value("login", QString("")).toString();
  this->_proxyPassword = settings.value("password", QString("")).toString();
  settings.endGroup();

  this->_password = unencrypt(this->_password);
  this->_proxyPassword = unencrypt(this->_proxyPassword);
}

void		Options::writeOptionSettings(void)
{
  QSettings	settings("Epitech", "QNetsoul");

  settings.beginGroup("MainOptions");
  settings.setValue("ip", this->_server);
  settings.setValue("port", this->_port);
  settings.setValue("login", this->_login);
  settings.setValue("location", this->_location);
  settings.setValue("comment", this->_comment);
  if (this->_savePassword)
    settings.setValue("password", encrypt(this->_password));
  else
    settings.remove("password");
  settings.setValue("savepassword", this->_savePassword);
  settings.endGroup();

  settings.beginGroup("ChatOptions");
  settings.setValue("exitonescape", this->_exitOnEscape);
  settings.setValue("typingnotification", this->_typingNotification);
  settings.setValue("smileys", this->_smileys);
  settings.setValue("replycomboboxvalue", this->_replyComboBoxValue);
  settings.setValue("replylocked", this->_replyLocked);
  settings.setValue("replyaway", this->_replyAway);
  settings.setValue("replyserver", this->_replyServer);
  settings.endGroup();

  settings.beginGroup("AdvancedOptions");
  settings.setValue("proxy", this->_proxy);
  settings.setValue("port", this->_proxyPort);
  settings.setValue("login", this->_proxyLogin);
  settings.setValue("password", encrypt(this->_proxyPassword));
  settings.setValue("useproxy", this->_useProxy);
  settings.endGroup();
}

void	Options::update(void)
{
  // Main Tab
  this->serverLineEdit->setText(this->_server);
  this->portLineEdit->setText(this->_port);
  this->loginLineEdit->setText(this->_login);
  this->passwordLineEdit->setText(this->_password);
  this->locationLineEdit->setText(this->_location);
  this->commentLineEdit->setText(this->_comment);
  if (this->_savePassword)
    this->savePasswordCheckBox->setCheckState(Qt::Checked);
  else
    this->savePasswordCheckBox->setCheckState(Qt::Unchecked);

  // Chat tab
  if (this->_exitOnEscape)
    this->chatEscapeCheckBox->setCheckState(Qt::Checked);
  else
    this->chatEscapeCheckBox->setCheckState(Qt::Unchecked);
  if (this->_typingNotification)
    this->typingStatusCheckBox->setCheckState(Qt::Checked);
  else
    this->typingStatusCheckBox->setCheckState(Qt::Unchecked);
  if (this->_smileys)
    this->smileysCheckBox->setCheckState(Qt::Checked);
  else
    this->smileysCheckBox->setCheckState(Qt::Unchecked);
  // Auto Reply
  this->autoReplyComboBox->setCurrentIndex(this->_replyComboBoxValue);
  loadReply(this->_replyComboBoxValue);

  // Advanced Tab
  this->proxyLineEdit->setText(this->_proxy);
  this->proxyPortLineEdit->setText(this->_proxyPort);
  this->proxyLoginLineEdit->setText(this->_proxyLogin);
  this->proxyPasswordLineEdit->setText(this->_proxyPassword);
  if (this->_useProxy)
    {
      this->proxyCheckBox->setCheckState(Qt::Checked);
      this->proxyLineEdit->setEnabled(true);
      this->proxyPortLineEdit->setEnabled(true);
      this->proxyLoginLineEdit->setEnabled(true);
      this->proxyPasswordLineEdit->setEnabled(true);
    }
  else
    this->proxyCheckBox->setCheckState(Qt::Unchecked);
}

bool	Options::isValidProxy(void) const
{
  bool	port_ok;
  this->_proxyPort.toUShort(&port_ok);

  return (!this->_proxy.isEmpty() &&
	  !this->_proxyLogin.isEmpty() &&
	  !this->_proxyPassword.isEmpty() &&
	  port_ok);
}

const QNetworkProxy	Options::getProxy(void) const
{
  bool		port_ok;
  quint16       port = this->_proxyPort.toUShort(&port_ok);

  QNetworkProxy	proxy(QNetworkProxy::HttpProxy,
		      this->_proxy, port, this->_proxyLogin,
		      this->_proxyPassword);
  return proxy;
}

void	Options::applyOptions(void)
{
  if (this->_useProxy)
    emit resetProxy(getProxy());
  else
    emit resetProxy();

  emit chatOptionsChanged(this->_exitOnEscape, this->_typingNotification,
			  this->_smileys);
}

QString	Options::getReply(int index)
{
  switch (index)
    {
    case 0: return this->_replyLocked;
    case 1: return this->_replyAway;
    case 2: return this->_replyServer;
    default:
      {
	std::cerr << "Error: bad index in getReply" << std::endl;
	std::cerr << "Value: " << index << std::endl;
      }
    }
  return "";
}

void	Options::save(void)
{
  const bool	proxyNeedsToBeReset =
    (this->_useProxy != (Qt::Checked == this->proxyCheckBox->checkState()));

  // Advanced Options
  this->_proxy = this->proxyLineEdit->text();
  this->_proxyPort = this->proxyPortLineEdit->text();
  this->_proxyLogin = this->proxyLoginLineEdit->text();
  this->_proxyPassword = this->proxyPasswordLineEdit->text();
  this->_useProxy = (Qt::Checked == this->proxyCheckBox->checkState());
  if (proxyNeedsToBeReset)
    {
      if (this->_useProxy)
	emit resetProxy(getProxy());
      else
	emit resetProxy();
    }

  // Chat Options
  this->_exitOnEscape = this->chatEscapeCheckBox->checkState();
  this->_typingNotification = this->typingStatusCheckBox->checkState();
  this->_smileys = this->smileysCheckBox->checkState();
  saveCurrentReply();
  emit chatOptionsChanged(this->_exitOnEscape, this->_typingNotification,
			  this->_smileys);

  // Main Options
  this->_server = this->serverLineEdit->text();
  this->_port = this->portLineEdit->text();
  this->_login = this->loginLineEdit->text();
  this->_password = this->passwordLineEdit->text();
  this->_location = this->locationLineEdit->text();
  this->_comment = this->commentLineEdit->text();
  this->_savePassword = (Qt::Checked == this->savePasswordCheckBox->checkState());
  if (!this->_login.isEmpty() && !this->_password.isEmpty() && this->_connectOnOk)
    emit loginPasswordFilled();
  this->_connectOnOk = false;
}

void	Options::loadReply(int index)
{
  if (this->_oldComboBoxValue != -42)
    saveCurrentReply(this->_oldComboBoxValue);

  switch (index)
    {
    case 0: this->autoReplyLineEdit->setText(this->_replyLocked); break;
    case 1: this->autoReplyLineEdit->setText(this->_replyAway); break;
    case 2: this->autoReplyLineEdit->setText(this->_replyServer); break;
    default:
      {
	std::cerr << "Error: bad index in loadReply" << std::endl;
	std::cerr << "Value: " << index << std::endl;
      }
    }
  this->_replyComboBoxValue = index;
  this->_oldComboBoxValue = index;
}

void	Options::saveCurrentReply(void)
{
  saveCurrentReply(this->autoReplyComboBox->currentIndex());
}

void	Options::saveCurrentReply(int index)
{
  switch (index)
    {
    case 0: this->_replyLocked = this->autoReplyLineEdit->text(); break;
    case 1: this->_replyAway = this->autoReplyLineEdit->text(); break;
    case 2: this->_replyServer = this->autoReplyLineEdit->text(); break;
    default:
      {
	std::cerr << "Error: bad index in saveCurrentReply" << std::endl;
      }
    }
}
