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

#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include <QMenu>
#include <QWidget>
#include <QMouseEvent>
#include "ConnectionPoint.h"
#include "ui_ContactWidget.h"

class	ContactWidget : public QWidget, public Ui_ContactWidget
{
  Q_OBJECT

    public:
  ContactWidget(QWidget* parent, const QString& login, const QString& alias = "");
  virtual ~ContactWidget(void);

  void		reset(void);
  int		getConnectionsSize(void) const;

  QString	getLogin(void) const;
  void		setLogin(const QString&);

  bool		hasGroup(void) const;
  QString	getGroup(void) const;
  void		setGroup(const QString&);

  QString	getStatus(void) const;
  void		addConnectionPoint(const ConnectionPoint&);
  void		updateConnectionPoint(const QString& id, const QString& state);
  void		buildToolTip(void);

 signals:
  void		doubleClick(const QString&, const QString&);

 protected:
  void		mousePressEvent(QMouseEvent* event);
  void		mouseDoubleClickEvent(QMouseEvent* event);

 private:
  void		updateState(void);
  QString	buildConnectionsPointToolTip(void) const;
  QString	buildLine(const QString& first = "", const QString& second = "") const;

  ContactWidget(const ContactWidget&);
  ContactWidget& operator=(const ContactWidget&);

  QString			_login;
  QString			_group;
  QString			_state;
  QString			_pixmap;
  QList<ConnectionPoint>	_connections;
};

#endif // CONTACTWIDGET_H
