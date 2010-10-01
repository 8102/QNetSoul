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

#include "OptionsWidget.h"
#include "OptionsContactsWidget.h"

OptionsContactsWidget::OptionsContactsWidget(QWidget* parent)
  : QWidget(parent), _contactBehavior(ContactBehavior::EXPAND)
{
}

OptionsContactsWidget::~OptionsContactsWidget(void)
{
}

void    OptionsContactsWidget::readOptions(QSettings& settings)
{
  settings.beginGroup("ContactsOptions");
  this->_contactBehavior =
    settings.value("contactBehavior", ContactBehavior::EXPAND).toInt();
  settings.endGroup();
}

void    OptionsContactsWidget::writeOptions(QSettings& settings)
{
  settings.beginGroup("ContactsOptions");
  settings.setValue("contactBehavior", this->_contactBehavior);
  settings.endGroup();
}

void    OptionsContactsWidget::updateOptions(void)
{
  this->_options->contactDoubleClikingBehaviorComboBox->
    setCurrentIndex(this->_contactBehavior);
}

void    OptionsContactsWidget::saveOptions(void)
{
  this->_contactBehavior =
    this->_options->contactDoubleClikingBehaviorComboBox->currentIndex();
}
