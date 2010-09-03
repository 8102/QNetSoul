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

#ifndef CHAT_H
#define CHAT_H

#include "ui_Chat.h"

class   Options;
class   Network;
class   QStatusBar;

class   Chat : public QWidget, public Ui_Chat
{
  Q_OBJECT

    public:
  Chat(const int id, const QString& name, const QString& location);
  virtual ~Chat(void);

  int     id(void) const { return this->_id; }
  QString login(void) const { return this->_login; }
  QString location(void) const { return this->_location; }

  void	  setAlias(const QString& alias) { this->_alias = alias; }
  void    setOptions(Options* options) { this->_options = options; }
  void    setNetwork(Network* network) { this->_network = network; }

  QString getFormatedDateTime(void) const;
  void    insertSmileys(void);
  void    replaceUrls(QString msg);
  void    insertMessage(const QString& l, const QString& m, const QColor&);
  void    notifyTypingStatus(const bool typing);
  void    setPortrait(void); // if existing
  void    autoReply(const int currentStatus);

 protected:
  void  keyPressEvent(QKeyEvent*);
  void  showEvent(QShowEvent*);
  void	hideEvent(QHideEvent*);
  void  closeEvent(QCloseEvent*);

  private slots:
  void  sendMessage(void);
  void  handleTypingSignal(void);

 private:
  int      _id;
  QString  _alias;
  QString  _login;
  QString  _location;
  QRect    _geometry;
  Options* _options;
  Network* _network;
};

#endif // CHAT_H
