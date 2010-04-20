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

#ifndef INPUT_TEXT_EDIT_H
#define INPUT_TEXT_EDIT_H

#include <QTextEdit>

class	InputTextEdit : public QTextEdit
{
  Q_OBJECT

  public:
  InputTextEdit(QWidget* parent = 0);
  ~InputTextEdit(void);

 protected:
  void	keyPressEvent(QKeyEvent* event);

 signals:
  void	returnPressed(void);
};

#endif // INPUT_TEXT_EDIT_H
