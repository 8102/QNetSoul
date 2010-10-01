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

#ifndef ABSTRACT_OPTIONS_H_
#define ABSTRACT_OPTIONS_H_

#include <QCheckBox>
#include <QSettings>

class   OptionsWidget;

class   AbstractOptions
{
 public:
  virtual void setOptions(OptionsWidget* options) { this->_options = options; }
  virtual void readOptions(QSettings& settings) = 0;
  virtual void writeOptions(QSettings& settings) = 0;
  virtual void updateOptions(void) = 0;
  virtual void saveOptions(void) = 0;

 protected:
  OptionsWidget* _options;
};

#endif
