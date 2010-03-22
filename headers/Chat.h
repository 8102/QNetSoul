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

#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include "ui_Chat.h"

class	QStatusBar;

class	Chat : public QWidget, public Ui_Chat
{
  Q_OBJECT

    public:
  Chat(const QString& name, bool exitOnEscape, bool smileys);
  virtual ~Chat(void);

  QString	getFormatedDateTime(void) const;
  void		insertSmileys(void);
  void		replaceUrls(QString msg);
  void		insertMessage(const QString& login, const QString& message, const QColor& color);
  void		notifyTypingStatus(const QString& login, bool typing);
  void		setExitOnEscape(bool b) { this->_exitOnEscape = b; }
  void		setSmileys(bool b) { this->_smileys = b; }

 signals:
  void	msgToSend(const QString&, const QString&);
  void	typingSignal(const QString&, bool);

 protected:
  void	keyPressEvent(QKeyEvent*);
  void	showEvent(QShowEvent*);
  void	closeEvent(QCloseEvent*);

  private slots:
  void	sendMessage(void);
  void	handleTypingSignal(void);

 private:
  QRect	_geometry;
  bool	_exitOnEscape;
  bool	_smileys;
};

#endif // CHAT_H
