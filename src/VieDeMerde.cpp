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

#include <iostream>
#include <QUrl>
#include <QRegExp>
#include <QString>
#include <QByteArray>
#include <QNetworkAccessManager>
#include "Url.h"
#include "VieDeMerde.h"
#include "QSlidingPopup.h"

namespace
{
  void  replaceHtmlSpecialChars(QString& str)
  {
    str.replace("&amp;", "&");
    str.replace("&#039;", "'");
    str.replace("&quot;", "\"");
    str.replace("&lt;", "<");
    str.replace("&gt;", ">");
  }
  const QUrl vdmUrl =
    QUrl("http://api.viedemerde.fr/1.2/view/random?key=readonly");
}

VieDeMerde::VieDeMerde(QSlidingPopup* popup) : _popup(popup)
{
  connect(this, SIGNAL(finished(QNetworkReply*)),
	  SLOT(replyFinished(QNetworkReply*)));
}

VieDeMerde::~VieDeMerde(void)
{
}

void    VieDeMerde::getVdm(void)
{
  get(QNetworkRequest(vdmUrl));
}

void    VieDeMerde::replyFinished(QNetworkReply* reply)
{
  Q_ASSERT(this->_popup);
  QByteArray array = reply->readAll();

  QString buffer(QString::fromUtf8(array));
  QRegExp textRegExp("<texte>(.+)</texte>");

  replaceHtmlSpecialChars(buffer);
  if (buffer.contains(textRegExp))
    {
      //std::cerr << textRegExp.cap(1).toStdString() << std::endl;
      this->_popup->showUp(textRegExp.cap(1), 20000);
    }
  reply->deleteLater();
}
