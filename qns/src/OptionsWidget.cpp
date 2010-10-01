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
#include <QSettings>
#include <QInputDialog>
#include "OptionsWidget.h"
#include "Encryption.h"

OptionsWidget::OptionsWidget(QWidget* parent) : QDialog(parent)
{
  setupUi(this);
  this->mainWidget->setOptions(this);
  this->contactsWidget->setOptions(this);
  this->chatWidget->setOptions(this);
  this->blockedWidget->setOptions(this);
  this->funWidget->setOptions(this);
  this->funWidget->init();
  this->proxyWidget->setOptions(this);
  readOptionSettings();
  updateOptions();
  connect(this, SIGNAL(accepted()), SLOT(save()));
}

OptionsWidget::~OptionsWidget(void)
{
}

void    OptionsWidget::readOptionSettings(void)
{
  QSettings settings("Epitech", "QNetsoul");

  this->mainWidget->readOptions(settings);
  this->contactsWidget->readOptions(settings);
  this->chatWidget->readOptions(settings);
  this->blockedWidget->readOptions(settings);
  this->funWidget->readOptions(settings);
  this->proxyWidget->readOptions(settings);
}

void    OptionsWidget::writeOptionSettings(void)
{
  QSettings settings("Epitech", "QNetsoul");

  this->mainWidget->writeOptions(settings);
  this->contactsWidget->writeOptions(settings);
  this->chatWidget->writeOptions(settings);
  this->blockedWidget->writeOptions(settings);
  this->funWidget->writeOptions(settings);
  this->proxyWidget->writeOptions(settings);
}

void    OptionsWidget::updateOptions(void)
{
  this->mainWidget->updateOptions();
  this->contactsWidget->updateOptions();
  this->chatWidget->updateOptions();
  this->blockedWidget->updateOptions();
  this->funWidget->updateOptions();
  this->proxyWidget->updateOptions();
}

// Order does matter
void    OptionsWidget::save(void)
{
  this->proxyWidget->saveOptions();
  this->funWidget->saveOptions();
  this->chatWidget->saveOptions();
  this->contactsWidget->saveOptions();
  this->mainWidget->saveOptions();
  writeOptionSettings();
}
