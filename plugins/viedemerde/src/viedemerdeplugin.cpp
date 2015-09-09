#include <QUrl>
#include <QIcon>
#include <QString>
#include <QtPlugin>
#include <QKeySequence>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include "tools.h"
#include "viedemerdeplugin.h"

namespace
{
  const int DefaultDuration = 20000;
  const QUrl& VdmUrl =
    QUrl("http://api.viedemerde.fr/1.2/view/random?key=readonly");
}

VieDeMerdePlugin::VieDeMerdePlugin(void)
  : _manager(new QNetworkAccessManager(this))
{
  connect(this->_manager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(handleFinishedRequests(QNetworkReply*)));
}

VieDeMerdePlugin::~VieDeMerdePlugin(void)
{
}

QString VieDeMerdePlugin::name(void)
{
  return "VieDeMerde";
}

QString VieDeMerdePlugin::title(void)
{
  return "&Vie de Merde";
}

QString VieDeMerdePlugin::version(void)
{
  return "1.0.0";
}

QString VieDeMerdePlugin::description(void)
{
  return "Ma vie c'est de la merde, et je vous emmerde !";
}

QIcon VieDeMerdePlugin::icon(void)
{
  return QIcon();
}

QKeySequence VieDeMerdePlugin::shortcut(void)
{
  return QKeySequence(Qt::Key_F6);
}

void VieDeMerdePlugin::fetchText(void)
{
  this->_manager->get(QNetworkRequest(VdmUrl));
}

void VieDeMerdePlugin::displayFetchedText(void)
{
  if (this->_currentVdm.isEmpty() == false)
    emit readyToDisplay(this->_currentVdm, DefaultDuration);
}

void VieDeMerdePlugin::displayLastFetchedText(void)
{
  if (this->_lastVdm.isEmpty() == false)
    emit readyToDisplay(this->_lastVdm, DefaultDuration);
}

void VieDeMerdePlugin::handleFinishedRequests(QNetworkReply* reply)
{
  if (reply->error() == QNetworkReply::NoError)
    {
      QString buffer(QString::fromUtf8(reply->readAll()));
      QRegExp textRegExp("<texte>(.+)</texte>");
      Tools::replaceHtmlSpecialChars(buffer);
      if (buffer.contains(textRegExp))
        {
	  if (this->_currentVdm.isEmpty() == false)
	    this->_lastVdm = this->_currentVdm;
	  this->_currentVdm = textRegExp.cap(1);
	  emit readyToDisplay(this->_currentVdm, DefaultDuration);
        }
    }
#ifndef QT_NO_DEBUG
  else
    {
      qDebug() << "[VieDeMerdePlugin::handleFinishedRequests]"
               << reply->errorString();
    }
#endif
  reply->deleteLater();
}
