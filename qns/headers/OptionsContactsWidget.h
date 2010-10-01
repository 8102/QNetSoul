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

#ifndef OPTIONS_CONTACTS_WIDGET_H_
#define OPTIONS_CONTACTS_WIDGET_H_

#include <QWidget>
#include "AbstractOptions.h"

// EXPAND is the default behavior
namespace ContactBehavior{ enum { EXPAND, OPEN_CONVERSATION, BOTH }; }

class   OptionsContactsWidget : public QWidget, public AbstractOptions
{
  Q_OBJECT

    public:
  OptionsContactsWidget(QWidget* parent = NULL);
  ~OptionsContactsWidget(void);

  // Behavior on double cliking on it.
  int contactBehavior(void) const { return this->_contactBehavior; }

  void readOptions(QSettings& settings);
  void writeOptions(QSettings& settings);
  void updateOptions(void);
  void saveOptions(void);

 private:
  int _contactBehavior;
};

#endif
