#ifndef CREDENTIALS_H_
#define CREDENTIALS_H_

#include <QObject>

class	Options;
class	QStringList;
class	QNetworkProxy;
class	QAuthenticator;
class	CredentialsDialog;

class   Credentials : public QObject
{
  Q_OBJECT

    public:
  Credentials(void);
  ~Credentials(void);

  void	setOptions(Options* options) { this->_options = options; }

  public slots:
  void	handleCredentials(const QNetworkProxy& proxy, QAuthenticator* auth);

 private:
  QStringList getCredentials(const QString& proxyHostName);

 private:
  Options*           _options;
  CredentialsDialog* _dialog;
};

#endif
