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

#ifndef INTERFACE_WIDGET_PLUGIN_H_
#define INTERFACE_WIDGET_PLUGIN_H_

#include <QObject>
#include "iplugindescriptor.h"

class IWidgetPlugin : public QObject, public IPluginDescriptor
{
  Q_OBJECT

  public:
  virtual ~IWidgetPlugin(void) {}

public slots:
  virtual void displayWidget(void) = 0; // Entry point
};

Q_DECLARE_INTERFACE(IWidgetPlugin, "QNetSoul.IWidgetPlugin/1.0");

#endif
