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

#include <QFile>
#include <QInputDialog>
#include "Options.h"
#include "Encryption.h"
#include "OptionsAdvancedWidget.h"

OptionsAdvancedWidget::OptionsAdvancedWidget(QWidget* parent)
  : QWidget(parent), _useProxy(false)
{
}

OptionsAdvancedWidget::~OptionsAdvancedWidget(void)
{
}

void	OptionsAdvancedWidget::applyOption(void)
{
  if (this->_useProxy)
    emit resetProxy(getProxy());
  else
    emit resetProxy();
}

void	OptionsAdvancedWidget::readOptions(QSettings& settings)
{
  settings.beginGroup("AdvancedOptions");
  this->_useProxy = settings.value("useproxy", bool(false)).toBool();
  this->_proxy = settings.value("proxy", QString("proxy.epitech.net")).toString();
  this->_proxyPort = settings.value("port", QString("3128")).toString();
  this->_proxyLogin = settings.value("login", QString("")).toString();
  this->_proxyPassword = settings.value("password", QString("")).toString();
  settings.endGroup();

  this->_proxyPassword = unencrypt(this->_proxyPassword);
}

void	OptionsAdvancedWidget::writeOptions(QSettings& settings)
{
  settings.beginGroup("AdvancedOptions");
  settings.setValue("proxy", this->_proxy);
  settings.setValue("port", this->_proxyPort);
  settings.setValue("login", this->_proxyLogin);
  settings.setValue("password", encrypt(this->_proxyPassword));
  settings.setValue("useproxy", this->_useProxy);
  settings.endGroup();
}

void	OptionsAdvancedWidget::updateOptions(void)
{
  // Advanced Tab
  this->_options->proxyLineEdit->setText(this->_proxy);
  this->_options->proxyPortLineEdit->setText(this->_proxyPort);
  this->_options->proxyLoginLineEdit->setText(this->_proxyLogin);
  this->_options->proxyPasswordLineEdit->setText(this->_proxyPassword);
  if (this->_useProxy)
    {
      this->_options->proxyCheckBox->setCheckState(Qt::Checked);
      this->_options->proxyLineEdit->setEnabled(true);
      this->_options->proxyPortLineEdit->setEnabled(true);
      this->_options->proxyLoginLineEdit->setEnabled(true);
      this->_options->proxyPasswordLineEdit->setEnabled(true);
    }
  else
    this->_options->proxyCheckBox->setCheckState(Qt::Unchecked);
}

void	OptionsAdvancedWidget::saveOptions(void)
{
  const bool    proxyNeedsToBeReset =
    (this->_useProxy !=
     (Qt::Checked == this->_options->proxyCheckBox->checkState()));

  this->_proxy = this->_options->proxyLineEdit->text();
  this->_proxyPort = this->_options->proxyPortLineEdit->text();
  this->_proxyLogin = this->_options->proxyLoginLineEdit->text();
  this->_proxyPassword = this->_options->proxyPasswordLineEdit->text();
  this->_useProxy =
    (Qt::Checked == this->_options->proxyCheckBox->checkState());
  if (proxyNeedsToBeReset)
    {
      if (this->_useProxy)
	emit resetProxy(getProxy());
      else
	emit resetProxy();
    }
}

bool    OptionsAdvancedWidget::isValidProxy(void) const
{
  bool  port_ok;
  this->_proxyPort.toUShort(&port_ok);

  return (!this->_proxy.isEmpty() &&
          !this->_proxyLogin.isEmpty() &&
          !this->_proxyPassword.isEmpty() &&
          port_ok);
}

const QNetworkProxy	OptionsAdvancedWidget::getProxy(void) const
{
  bool          port_ok;
  quint16       port = this->_proxyPort.toUShort(&port_ok);

  QNetworkProxy proxy(QNetworkProxy::HttpProxy,
                      this->_proxy, port, this->_proxyLogin,
                      this->_proxyPassword);
  return proxy;
}
