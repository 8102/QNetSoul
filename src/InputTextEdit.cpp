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

#include <iostream>
#include <QKeyEvent>
#include "InputTextEdit.h"

InputTextEdit::InputTextEdit(QWidget* parent) : QTextEdit(parent)
{
}

InputTextEdit::~InputTextEdit(void)
{
}

void	InputTextEdit::keyPressEvent(QKeyEvent* event)
{
  if (Qt::Key_Return == event->key())
    {
      if (Qt::ControlModifier == event->modifiers())
        {
	  QKeyEvent	returnEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, "Return");
	  QTextEdit::keyPressEvent(&returnEvent);
        }
      else
        {
	  emit returnPressed();
        }
    }
  else if (Qt::Key_Enter == event->key()) // numpad
    {
      if ((Qt::ControlModifier | Qt::KeypadModifier) == event->modifiers())
	{
	  QKeyEvent	returnEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, "Return");
	  QTextEdit::keyPressEvent(&returnEvent);
	}
      else
	{
	  emit returnPressed();
	}
    }
  else
    {
      QTextEdit::keyPressEvent(event);
    }
}
