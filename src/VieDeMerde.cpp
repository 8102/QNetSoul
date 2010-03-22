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

#include <iostream>
#include <QUrl>
#include <QRegExp>
#include <QString>
#include <QByteArray>
#include <QNetworkAccessManager>
#include "Url.h"
#include "VieDeMerde.h"

VieDeMerde::VieDeMerde(void)
{
  this->_manager = new QNetworkAccessManager(this);
  connect(this->_manager, SIGNAL(finished(QNetworkReply*)),
	  SLOT(replyFinished(QNetworkReply*)));
}

VieDeMerde::~VieDeMerde(void)
{
}

void	VieDeMerde::getALife(void)
{
  this->_manager->get(QNetworkRequest(QUrl("http://api.viedemerde.fr/1.2/view/random?key=readonly")));
}

/*
  01/03/2010
  Trouver l'equivalent en Qt... parce que c'est trop lame cette fonction...
*/
static void	replaceHtmlSpecialChars(QString& str)
{
  str.replace("&amp;", "&");
  str.replace("&#039;", "'");
  str.replace("&quot;", "\"");
  str.replace("&lt;", "<");
  str.replace("&gt;", ">");
}

void	VieDeMerde::replyFinished(QNetworkReply* reply)
{
  QByteArray	array = reply->readAll();


  QString	buffer(QString::fromUtf8(array));
  QRegExp	textRegExp("<texte>(.+)</texte>");

  replaceHtmlSpecialChars(buffer);
  if (buffer.contains(textRegExp))
    {
      //std::cerr << textRegExp.cap(1).toStdString() << std::endl;
      emit sendVdmToQNetsoul(textRegExp.cap(1));
    }
  reply->deleteLater();
}
