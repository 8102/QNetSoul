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

#ifndef OPTIONS_CHAT_WIDGET_H_
#define OPTIONS_CHAT_WIDGET_H_

#include <QWidget>
#include "OptionsWidget.h"

class	OptionsChatWidget : public QWidget, public OptionsWidget
{
  Q_OBJECT

  public:
  OptionsChatWidget(QWidget* parent = 0);
  ~OptionsChatWidget(void);

  void	applyOption(void);
  void	setOptions(Options* options);
  void	readOptions(QSettings& settings);
  void	writeOptions(QSettings& settings);
  void	updateOptions(void);
  void	saveOptions(void);
  QString getReply(int index);

public slots:
  void	loadReply(int index);

signals:
  void  chatOptionsChanged(bool, bool, bool);

private:
  void	saveCurrentReply(void);
  void	saveCurrentReply(int index);

private:
  bool		_exitOnEscape;
  bool		_typingNotification;
  bool		_smileys;
  int		_oldComboBoxValue;
  int		_replyComboBoxValue;
  QString	_replyLocked;
  QString	_replyAway;
  QString	_replyServer;
};

#endif
