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

#ifndef CHUCK_NORRIS_FACTS_H_
#define CHUCK_NORRIS_FACTS_H_

#include <QObject>
#include <QStringList>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class   Popup;

class   ChuckNorrisFacts : public QObject
{
  Q_OBJECT

  public:
  ChuckNorrisFacts(Popup* popup);
  virtual ~ChuckNorrisFacts(void);

  void  setProxy(const QNetworkProxy& p) { this->_manager->setProxy(p); }

public slots:
  void  getFact(void);

private:
  void  pickAFact(void);

private slots:
  void  replyFinished(QNetworkReply* reply);

private:
  Popup*                  _popup;
  QStringList             _facts;
  QNetworkAccessManager*  _manager;
};

#endif // VIEDEMERDE_H
