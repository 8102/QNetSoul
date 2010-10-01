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

#include <QSettings>
#include "QNetsoul.h"
#include "Encryption.h"
#include "OptionsWidget.h"
#include "OptionsMainWidget.h"

OptionsMainWidget::OptionsMainWidget(QWidget* parent)
  : QWidget(parent), _savePassword(false),
    _autoConnect(false), _connectOnOk(false)
{
}

OptionsMainWidget::~OptionsMainWidget(void)
{
}

void	OptionsMainWidget::readOptions(QSettings& settings)
{
  settings.beginGroup("MainOptions");
  this->_server = settings.value("ip", "ns-server.epita.fr").toString();
  this->_port = settings.value("port", "4242").toString();
  this->_login = settings.value("login").toString();
  this->_location = settings.value("location", "%L").toString();
  this->_comment = settings.value("comment").toString();
  this->_password = settings.value("password").toString();
  this->_savePassword = settings.value("savepassword", false).toBool();
  this->_autoConnect = settings.value("autoconnect", false).toBool();
  settings.endGroup();

  this->_password = unencrypt(this->_password);
}

void	OptionsMainWidget::writeOptions(QSettings& settings)
{
  settings.beginGroup("MainOptions");
  settings.setValue("ip", this->_server);
  settings.setValue("port", this->_port);
  settings.setValue("login", this->_login);
  settings.setValue("location", this->_location);
  settings.setValue("comment", this->_comment);
  (this->_savePassword) ?
    settings.setValue("password", encrypt(this->_password)) :
    settings.remove("password");
  settings.setValue("savepassword", this->_savePassword);
  settings.setValue("autoconnect", this->_autoConnect);
  settings.endGroup();
}

void	OptionsMainWidget::updateOptions(void)
{
  Q_ASSERT(this->_options);

  this->_options->serverLineEdit->setText(this->_server);
  this->_options->portLineEdit->setText(this->_port);
  this->_options->loginLineEdit->setText(this->_login);
  this->_options->passwordLineEdit->setText(this->_password);
  this->_options->locationLineEdit->setText(this->_location);
  this->_options->commentLineEdit->setText(this->_comment);
  this->_options->savePasswordCheckBox->setChecked(this->_savePassword);
  this->_options->autoConnectCheckBox->setChecked(this->_autoConnect);
}

void	OptionsMainWidget::saveOptions(void)
{
  Q_ASSERT(this->_options);

  this->_server = this->_options->serverLineEdit->text();
  this->_port = this->_options->portLineEdit->text();
  this->_login = this->_options->loginLineEdit->text();
  this->_password = this->_options->passwordLineEdit->text();
  this->_location = this->_options->locationLineEdit->text();
  this->_comment = this->_options->commentLineEdit->text();
  this->_savePassword = this->_options->savePasswordCheckBox->isChecked();
  this->_autoConnect = this->_options->autoConnectCheckBox->isChecked();
  if (!this->_login.isEmpty() &&
      !this->_password.isEmpty() &&
      this->_connectOnOk)
    emit loginPasswordFilled();
  this->_connectOnOk = false;
}
