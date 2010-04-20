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
#include "Randn.h"
#include "ChuckNorrisFacts.h"

const char*	facts_url =
  "http://www.chucknorrisfacts.fr/fortunes/fortunes.txt";

ChuckNorrisFacts::ChuckNorrisFacts(void)
{
  this->_manager = new QNetworkAccessManager(this);
  connect(this->_manager, SIGNAL(finished(QNetworkReply*)),
	  this, SLOT(replyFinished(QNetworkReply*)));
}

ChuckNorrisFacts::~ChuckNorrisFacts(void)
{
}

void	ChuckNorrisFacts::getFact(void)
{
  if (0 == this->_facts.size())
    this->_manager->get(QNetworkRequest(QUrl(facts_url)));
  else
    pickAFact();
}

void	ChuckNorrisFacts::pickAFact(void)
{
  const int	size = this->_facts.size();

  if (size)
    {
      std::cerr << this->_facts.at(rand_n(size)).toStdString() << std::endl;
      //emit sendChuckNorrisFactToQNetsoul(this->_facts.at(rand_n(size)));
    }
}

static void	replaceHtmlSpecialChars(QString& str)
{
  str.replace("&amp;", "&");
  str.replace("&#039;", "'");
  str.replace("&quot;", "\"");
  str.replace("&lt;", "<");
  str.replace("&gt;", ">");
  str.replace("&agrave", "à");
}

void	ChuckNorrisFacts::replyFinished(QNetworkReply* reply)
{
  QByteArray	array = reply->readAll();
  QString	buffer(QString::fromUtf8(array));

  replaceHtmlSpecialChars(buffer);
  this->_facts = buffer.split("%\n", QString::SkipEmptyParts);
  pickAFact();
  reply->deleteLater();
}
