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

#ifndef INTERFACE_POPUP_PLUGIN_H_
#define INTERFACE_POPUP_PLUGIN_H_

#include <QObject>
#include "iplugindescriptor.h"

class QString;

class IPopupPlugin : public QObject, public IPluginDescriptor
{
  Q_OBJECT

  public:
  virtual ~IPopupPlugin(void) {}

public slots:
  virtual void fetchText(void) = 0; // Entry point
  virtual void displayFetchedText(void) = 0;
  virtual void displayLastFetchedText(void) = 0;

signals:
  void readyToDisplay(QString text, const int ms);
};

Q_DECLARE_INTERFACE(IPopupPlugin, "QNetSoul.IPopupPlugin/1.0");

#endif
