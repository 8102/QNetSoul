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
#include <QStringList>
#include <QNetworkProxy>
#include <QAuthenticator>
#include "Credentials.h"
#include "CredentialsDialog.h"

// Example:
// Credentials& instance = Singleton<Credentials>::Instance();

Credentials::Credentials(void)
{
  this->_dialog = new CredentialsDialog;
}

Credentials::~Credentials(void)
{
  this->_dialog->deleteLater();
}

void    Credentials::handleCredentials(const QNetworkProxy& proxy,
                                       QAuthenticator* auth)
{
  const QStringList credentials = getCredentials(proxy.hostName());
  if (credentials.size() == 2)
    {
      auth->setUser(credentials.at(0));
      auth->setPassword(credentials.at(1));
    }
}

QStringList     Credentials::getCredentials(const QString& proxyHostName)
{
  QStringList   credentials;
  // If accepted
  this->_dialog->setWindowTitle(proxyHostName);
  if (this->_dialog->exec() == 1)
    {
      credentials << this->_dialog->loginLineEdit->text()
                  << this->_dialog->passwordLineEdit->text();
    }
  return credentials;
}
