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
#include "OptionsBlockedWidget.h"

namespace
{
  const QString ConfigDirName = "config";
}

OptionsBlockedWidget::OptionsBlockedWidget(QWidget* parent)
  : QWidget(parent), _configDir(QDir::currentPath())
{
  setupConfigDir();
}

OptionsBlockedWidget::~OptionsBlockedWidget(void)
{
}

void	OptionsBlockedWidget::setOptions(Options* options)
{
  OptionsWidget::setOptions(options);
  connect(this->_options->addButton,
	  SIGNAL(clicked()), SLOT(addBlockedContact()));
  connect(this->_options->deleteButton,
	  SIGNAL(clicked()), SLOT(deleteBlockedContact()));
  connect(this->_options->deleteAllButton,
	  SIGNAL(clicked()), SLOT(deleteAllBlockedContacts()));
}

void	OptionsBlockedWidget::readOptions(QSettings& /*settings*/)
{
}

void	OptionsBlockedWidget::writeOptions(QSettings& /*settings*/)
{
}

void	OptionsBlockedWidget::updateOptions(void)
{
}

void	OptionsBlockedWidget::saveOptions(void)
{
}

void	OptionsBlockedWidget::addBlockedContact(void)
{
  QString login =
    QInputDialog::getText(this, tr("Block login"), tr("Login to block:"));

  if (login.isEmpty())
    return;

  addBlockedContact(login);
}

void	OptionsBlockedWidget::addBlockedContact(const QString& login)
{
  // if login isnt found in the list, add it.
  if (!this->_options->listWidget->findItems(login, Qt::MatchFixedString).size())
    {
      this->_options->listWidget->insertItem(this->_options->listWidget->count(), login);
      this->_options->deleteButton->setEnabled(true);
      this->_options->deleteAllButton->setEnabled(true);
      saveBlockedContacts();
    }
}

void	OptionsBlockedWidget::deleteBlockedContact(void)
{
  if (this->_options->listWidget->currentItem())
    {
      delete this->_options->listWidget->takeItem(this->_options->listWidget->currentRow());
      if (this->_options->listWidget->count() == 0)
        {
          this->_options->deleteButton->setEnabled(false);
          this->_options->deleteAllButton->setEnabled(false);
        }
      saveBlockedContacts();
    }
}

void	OptionsBlockedWidget::deleteAllBlockedContacts(void)
{
  this->_options->listWidget->clear();
  this->_options->deleteButton->setEnabled(false);
  this->_options->deleteAllButton->setEnabled(false);
  saveBlockedContacts();
}

void	OptionsBlockedWidget::setupConfigDir(void)
{
  if (!this->_configDir.exists(ConfigDirName))
    this->_configDir.mkdir(ConfigDirName);
  this->_configDir.cd(ConfigDirName);
}

void	OptionsBlockedWidget::saveBlockedContacts(void)
{
  QFile file(ConfigDirName + "/blockedlist.txt");

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  const int     size = this->_options->listWidget->count();
  QTextStream   out(&file);

  for (int i = 0; i < size; ++i)
    out << this->_options->listWidget->item(i)->text() << '\n';
  file.close();
}

void	OptionsBlockedWidget::loadBlockedContacts(void)
{
  QFile file(ConfigDirName + "/blockedlist.txt");

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QTextStream in(&file);
  while (!in.atEnd())
    addBlockedContact(in.readLine());
  file.close();
}
