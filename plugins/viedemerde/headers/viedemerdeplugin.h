#ifndef VIE_DE_MERDE_PLUGIN_H_
#define VIE_DE_MERDE_PLUGIN_H_

#include "ipopupplugin.h"

class QString;
class QNetworkReply;
class QNetworkAccessManager;

class VieDeMerdePlugin : public IPopupPlugin
{
  Q_OBJECT
  Q_INTERFACES(IPopupPlugin)

    public:
    VieDeMerdePlugin(void);
  virtual ~VieDeMerdePlugin(void);

  virtual QString name(void);
  virtual QString title(void);
  virtual QString version(void);
  virtual QString description(void);
  virtual QIcon icon(void);
  virtual QKeySequence shortcut(void);

public slots:
  virtual void fetchText(void);
  virtual void displayFetchedText(void);
  virtual void displayLastFetchedText(void);

private slots:
  void handleFinishedRequests(QNetworkReply* reply);

private:
  QString _lastVdm;
  QString _currentVdm;
  QNetworkAccessManager* _manager;
};

#endif
