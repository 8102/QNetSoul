#ifndef CREDENTIALS_H_
#define CREDENTIALS_H_

#include <QObject>

class	QStringList;
class	QNetworkProxy;
class	OptionsWidget;
class	QAuthenticator;
class	CredentialsDialog;

class   Credentials : public QObject
{
  Q_OBJECT

    public:
  Credentials(void);
  ~Credentials(void);

  void	setOptions(OptionsWidget* options) { this->_options = options; }

  public slots:
  void	handleCredentials(const QNetworkProxy& proxy, QAuthenticator* auth);

 private:
  QStringList getCredentials(const QString& proxyHostName);

 private:
  OptionsWidget*     _options;
  CredentialsDialog* _dialog;
};

#endif
