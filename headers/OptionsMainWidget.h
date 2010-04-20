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

#ifndef OPTION_MAIN_WIDGET_H_
#define OPTION_MAIN_WIDGET_H_

#include <QWidget>
#include "OptionsWidget.h"

class	OptionsMainWidget : public QWidget, public OptionsWidget
{
  Q_OBJECT

  public:
  OptionsMainWidget(QWidget* parent = 0);
  ~OptionsMainWidget(void);

  bool	autoConnect(void) const { return this->_autoConnect; }
  void	setConnectionOnOk(const bool& value) { this->_connectOnOk = value; }

  void	readOptions(QSettings& settings);
  void	writeOptions(QSettings& settings);
  void	updateOptions(void);
  void	saveOptions(void);

signals:
  void	loginPasswordFilled(void);

private:
  QString	_server;
  QString	_port;
  QString	_login;
  QString	_location;
  QString	_comment;
  QString	_password;
  bool		_savePassword;
  bool		_autoConnect;
  bool		_connectOnOk;
};

#endif
