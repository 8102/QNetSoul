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

#ifndef OPTIONS_ADVANCED_WIDGET_H_
#define OPTIONS_ADVANCED_WIDGET_H_

#include <QDir>
#include <QWidget>
#include <QNetworkProxy>
#include "OptionsWidget.h"

class	OptionsAdvancedWidget : public QWidget, public OptionsWidget
{
  Q_OBJECT

public:
  OptionsAdvancedWidget(QWidget* parent = 0);
  ~OptionsAdvancedWidget(void);

  bool	isProxyEnabled(void) const { return this->_useProxy; }

  void	applyOption(void);
  void	readOptions(QSettings& settings);
  void	writeOptions(QSettings& settings);
  void	updateOptions(void);
  void	saveOptions(void);
  bool	isValidProxy(void) const;
  const QNetworkProxy getProxy(void) const;

signals:
  void	resetProxy(const QNetworkProxy& p = QNetworkProxy());

private:
  bool		_useProxy;
  QString	_proxy;
  QString	_proxyPort;
  QString	_proxyLogin;
  QString	_proxyPassword;
};

#endif
