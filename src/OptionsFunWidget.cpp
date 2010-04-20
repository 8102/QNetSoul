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

#include <QFile>
#include <QInputDialog>
#include "Options.h"
#include "OptionsFunWidget.h"

OptionsFunWidget::OptionsFunWidget(QWidget* parent)
  : QWidget(parent), _startingVdm(false)
{
}

OptionsFunWidget::~OptionsFunWidget(void)
{
}

void	OptionsFunWidget::readOptions(QSettings& settings)
{
  settings.beginGroup("FunOptions");
  this->_startingVdm = settings.value("startingVdm", bool(false)).toBool();
  settings.endGroup();
}

void	OptionsFunWidget::writeOptions(QSettings& settings)
{
  settings.beginGroup("FunOptions");
  settings.setValue("startingVdm", this->_startingVdm);
  settings.endGroup();
}

void	OptionsFunWidget::updateOptions(void)
{
  setCheckState(this->_options->startingVdmCheckBox, this->_startingVdm);
}

void	OptionsFunWidget::saveOptions(void)
{
  this->_startingVdm = this->_options->startingVdmCheckBox->checkState();
}
