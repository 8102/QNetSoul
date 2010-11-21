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

#ifndef INTERFACE_PLUGIN_DESCRIPTOR_H_
#define INTERFACE_PLUGIN_DESCRIPTOR_H_

class QIcon;
class QString;
class QKeySequence;

class IPluginDescriptor
{
public:
  virtual ~IPluginDescriptor(void) {}
  virtual QString name(void) = 0; // unique name (intern)
  virtual QString title(void) = 0; // displayed text
  virtual QString version(void) = 0; // X.X.X
  virtual QString description(void) = 0;
  virtual QIcon icon(void) = 0;
  virtual QKeySequence shortcut(void) = 0;
};

#endif
