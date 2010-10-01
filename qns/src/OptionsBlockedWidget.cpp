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
#include "OptionsBlockedWidget.h"

OptionsBlockedWidget::OptionsBlockedWidget(QWidget* parent) : QWidget(parent)
{
}

OptionsBlockedWidget::~OptionsBlockedWidget(void)
{
}

void    OptionsBlockedWidget::setOptions(OptionsWidget* options)
{
  AbstractOptions::setOptions(options);
  connect(this->_options->addButton,
          SIGNAL(clicked()), SLOT(addBlockedContact()));
  connect(this->_options->deleteButton,
          SIGNAL(clicked()), SLOT(deleteBlockedContact()));
  connect(this->_options->deleteAllButton,
          SIGNAL(clicked()), SLOT(deleteAllBlockedContacts()));
}

void    OptionsBlockedWidget::readOptions(QSettings& settings)
{
  Q_UNUSED(settings);
}

void    OptionsBlockedWidget::writeOptions(QSettings& settings)
{
  Q_UNUSED(settings);
}

void    OptionsBlockedWidget::updateOptions(void)
{
}

void    OptionsBlockedWidget::saveOptions(void)
{
}

QStringList OptionsBlockedWidget::getList(void) const
{
  QStringList list;
  const int size = this->_options->listWidget->count();
  for (int i = 0; i < size; ++i)
    list << this->_options->listWidget->item(i)->text();
  return list;
}

bool    OptionsBlockedWidget::isBlocked(const QString& contact) const
{
  const int size = this->_options->listWidget->count();
  for (int i = 0; i < size; ++i)
    if (contact == this->_options->listWidget->item(i)->text())
      return true;
  return false;
}

void    OptionsBlockedWidget::addBlockedContact(const QString& login)
{
  // if login isnt found in the list, add it.
  if (!this->_options->listWidget->findItems(login, Qt::MatchFixedString).size())
    {
      this->_options->listWidget->insertItem(this->_options->listWidget->count(), login);
      this->_options->deleteButton->setEnabled(true);
      this->_options->deleteAllButton->setEnabled(true);
    }
}

void    OptionsBlockedWidget::addBlockedContact(void)
{
  const QString login =
    QInputDialog::getText(this, tr("Block login"), tr("Login to block:"));
  if (login.isEmpty())
    return;
  addBlockedContact(login);
}

void    OptionsBlockedWidget::deleteBlockedContact(void)
{
  int row;
  const QList<QListWidgetItem*> selected =
    this->_options->listWidget->selectedItems();

  if (selected.size() > 0)
    {
      for (int i = 0; i < selected.size(); ++i)
        {
          row = this->_options->listWidget->row(selected.at(i));
          delete this->_options->listWidget->takeItem(row);
        }
      if (this->_options->listWidget->count() == 0)
        {
          this->_options->deleteButton->setEnabled(false);
          this->_options->deleteAllButton->setEnabled(false);
        }
    }
}

void    OptionsBlockedWidget::deleteAllBlockedContacts(void)
{
  this->_options->listWidget->clear();
  this->_options->deleteButton->setEnabled(false);
  this->_options->deleteAllButton->setEnabled(false);
}
