#include <QDebug>
#include <QStringList>
#include <QNetworkProxy>
#include <QAuthenticator>
#include "Options.h"
#include "Credentials.h"
#include "CredentialsDialog.h"

// Example:
// Credentials& instance = Singleton<Credentials>::Instance();

Credentials::Credentials(void) : QObject(NULL), _options(NULL)
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
  if (this->_options)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[Credentials::handleCredentials]"
               << "Options are available.";
#endif
      const QString user = this->_options->proxyLoginLineEdit->text();
      const QString password = this->_options->proxyPasswordLineEdit->text();
      if (!user.isEmpty() && !password.isEmpty())
        {
          auth->setUser(user);
          auth->setPassword(password);
          return;
        }
    }
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
