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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDir>
#include <QDialog>
#include "ui_Options.h"

class Options : public QDialog, public Ui_Options
{
  Q_OBJECT

  public:
  Options(QWidget* parent);
  ~Options(void);

  void	readOptionSettings(void);
  void	writeOptionSettings(void);
  void	updateOptions(void);

private slots:
  void	save(void);
};

#endif // OPTIONS_H