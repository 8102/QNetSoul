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

#include <QUrl>
#include <QRegExp>
#include <QNetworkReply>
#include "Url.h"
#include "VieDeMerde.h"
#include "Popup.h"

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

VieDeMerde::VieDeMerde(Popup* popup) : _popup(popup)
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
  QString buffer(QString::fromUtf8(reply->readAll()));
  QRegExp textRegExp("<texte>(.+)</texte>");

  replaceHtmlSpecialChars(buffer);
  if (buffer.contains(textRegExp))
    {
      //qDebug() << textRegExp.cap(1);
      this->_popup->showUp(textRegExp.cap(1), 20000);
      this->_lastVdm = textRegExp.cap(1);
    }
  reply->deleteLater();
}
