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

#include <QSettings>
#include "Options.h"

Options::Options(QWidget* parent) : QDialog(parent), _connectOnOk(false)
{
  setupUi(this);
  readOptionSettings();
  update();
  connect(this, SIGNAL(accepted()), SLOT(save()));
}

Options::~Options(void)
{
}

void		Options::readOptionSettings(void)
{
  QSettings	settings("Epitech", "QNetsoul");

  settings.beginGroup("Options");
  this->_server = settings.value("ip", QString("ns-server.epita.fr")).toString();
  this->_port = settings.value("port", QString("4242")).toString();
  this->_login = settings.value("login", QString("")).toString();
  this->_location = settings.value("location", QString("%L")).toString();
  this->_comment = settings.value("comment", QString("-=[QNetSoul]=-")).toString();
  this->_password = settings.value("password", QString("")).toString();
  this->_savePassword = settings.value("savepassword", bool(false)).toBool();
  settings.endGroup();
}

void		Options::writeOptionSettings(void)
{
  QSettings	settings("Epitech", "QNetsoul");

  settings.beginGroup("Options");
  settings.setValue("ip", this->_server);
  settings.setValue("port", this->_port);
  settings.setValue("login", this->_login);
  settings.setValue("location", this->_location);
  settings.setValue("comment", this->_comment);

  if (true == this->_savePassword)
    {
      settings.setValue("password", this->_password);
      settings.setValue("savepassword", true);
    }
  else
    {
      settings.remove("password");
      settings.setValue("savepassword", false);
    }
  settings.endGroup();
}

void	Options::update(void)
{
  this->serverLineEdit->setText(this->_server);
  this->portLineEdit->setText(this->_port);
  this->loginLineEdit->setText(this->_login);
  this->passwordLineEdit->setText(this->_password);
  this->locationLineEdit->setText(this->_location);
  this->commentLineEdit->setText(this->_comment);
  if (this->_savePassword == true)
    this->savePasswordCheckBox->setCheckState(Qt::Checked);
  else
    this->savePasswordCheckBox->setCheckState(Qt::Unchecked);
}

void	Options::save(void)
{
  this->_server = this->serverLineEdit->text();
  this->_port = this->portLineEdit->text();
  this->_login = this->loginLineEdit->text();
  this->_password = this->passwordLineEdit->text();
  this->_location = this->locationLineEdit->text();
  this->_comment = this->commentLineEdit->text();
  if (Qt::Checked == this->savePasswordCheckBox->checkState())
    this->_savePassword = true;
  else
    this->_savePassword = false;
  if (!this->_login.isEmpty() && !this->_password.isEmpty() && this->_connectOnOk)
    {
      emit loginPasswordFilled();
    }
  this->_connectOnOk = false;
}
