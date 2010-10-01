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
#include "OptionsWidget.h"
#include "OptionsFunWidget.h"

OptionsFunWidget::OptionsFunWidget(QWidget* parent)
  : QWidget(parent), _startWith(tr("Nothing"))
{
}

OptionsFunWidget::~OptionsFunWidget(void)
{
}

void    OptionsFunWidget::init(void)
{
  Q_ASSERT(this->_options->nothingRadioButton);
  Q_ASSERT(this->_options->vdmRadioButton);
  Q_ASSERT(this->_options->cnfRadioButton);

  this->_choices << this->_options->nothingRadioButton
                 << this->_options->vdmRadioButton
                 << this->_options->cnfRadioButton;
}

void    OptionsFunWidget::readOptions(QSettings& settings)
{
  settings.beginGroup("FunOptions");
  this->_startWith = settings.value("startWith", tr("Nothing")).toString();
  settings.endGroup();
}

void    OptionsFunWidget::writeOptions(QSettings& settings)
{
  settings.beginGroup("FunOptions");
  settings.setValue("startWith", this->_startWith);
  settings.endGroup();
}

void    OptionsFunWidget::updateOptions(void)
{
  for (int i = 0; i < this->_choices.size(); ++i)
    {
      if (this->_startWith == this->_choices.at(i)->text())
	{
	  this->_choices.at(i)->setChecked(true);
	}
    }
}

void    OptionsFunWidget::saveOptions(void)
{
  for (int i = 0; i < this->_choices.size(); ++i)
    {
      if (this->_choices.at(i)->isChecked())
        this->_startWith = this->_choices.at(i)->text();
    }
}
