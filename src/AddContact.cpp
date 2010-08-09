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

#include <QMessageBox>
#include "AddContact.h"

AddContact::AddContact(QWidget* parent) : QDialog(parent)
{
  setupUi(this);
  connect(this->addPushButton, SIGNAL(clicked()), SLOT(addContact()));
}

AddContact::~AddContact(void)
{
}

void    AddContact::setGroups(const QStringList& groups)
{
  this->groupComboBox->clear();
  this->groupComboBox->addItem(tr("None"));
  for (int i = 0; i < groups.size(); ++i)
    this->groupComboBox->addItem(groups.at(i));
}

void	AddContact::reset(void)
{
  this->loginLineEdit->clear();
  this->aliasLineEdit->clear();
  this->loginLineEdit->setFocus();
  repaint();
}

void	AddContact::addContact(void)
{
  QStringList properties;

  if (this->loginLineEdit->text() == "")
    {
      QMessageBox::information(this, tr("Add Contact"),
			       tr("You should fill login field "
				  "before adding contact :)"));
      return;
    }
  properties << this->groupComboBox->currentText()
	     << this->loginLineEdit->text();
  if (this->aliasLineEdit->text() == "")
    properties << this->loginLineEdit->text();
  else
    properties << this->aliasLineEdit->text();
  emit newContact(properties);
  reset();
  if (Qt::Unchecked == this->closingCheckBox->checkState())
    this->hide();
}
