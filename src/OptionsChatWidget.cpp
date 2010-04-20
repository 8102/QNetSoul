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
#include "Options.h"
#include "Encryption.h"
#include "OptionsChatWidget.h"

OptionsChatWidget::OptionsChatWidget(QWidget* parent)
  : QWidget(parent), _exitOnEscape(false),
    _typingNotification(false), _smileys(false),
    _oldComboBoxValue(-42), _replyComboBoxValue(0)
{
}

OptionsChatWidget::~OptionsChatWidget(void)
{
}

void	OptionsChatWidget::applyOption(void)
{
  emit chatOptionsChanged(this->_exitOnEscape,
			  this->_typingNotification,
			  this->_smileys);
}

void	OptionsChatWidget::setOptions(Options* options)
{
  OptionsWidget::setOptions(options);
  connect(this->_options->autoReplyComboBox,
	  SIGNAL(currentIndexChanged(int)), SLOT(loadReply(int)));
}

void	OptionsChatWidget::readOptions(QSettings& settings)
{
  settings.beginGroup("ChatOptions");
  this->_exitOnEscape = settings.value("exitonescape", bool(false)).toBool();
  this->_typingNotification = settings.value("typingnotification", bool(false)).toBool();
  this->_smileys = settings.value("smileys", bool(false)).toBool();
  this->_replyComboBoxValue = settings.value("replycomboboxvalue", int(0)).toInt();
  this->_replyLocked = settings.value("replylocked", QString("[Locked]")).toString();
  this->_replyAway = settings.value("replyaway", QString("[Away]")).toString();
  this->_replyServer = settings.value("replyserver", QString("[Daemonized]")).toString();
  settings.endGroup();
}

void	OptionsChatWidget::writeOptions(QSettings& settings)
{
  settings.beginGroup("ChatOptions");
  settings.setValue("exitonescape", this->_exitOnEscape);
  settings.setValue("typingnotification", this->_typingNotification);
  settings.setValue("smileys", this->_smileys);
  settings.setValue("replycomboboxvalue", this->_replyComboBoxValue);
  settings.setValue("replylocked", this->_replyLocked);
  settings.setValue("replyaway", this->_replyAway);
  settings.setValue("replyserver", this->_replyServer);
  settings.endGroup();
}

void	OptionsChatWidget::updateOptions(void)
{
  setCheckState(this->_options->chatEscapeCheckBox, this->_exitOnEscape);
  setCheckState(this->_options->typingStatusCheckBox, this->_typingNotification);
  setCheckState(this->_options->smileysCheckBox, this->_smileys);
  this->_options->autoReplyComboBox->setCurrentIndex(this->_replyComboBoxValue);
  loadReply(this->_replyComboBoxValue);
}

void	OptionsChatWidget::saveOptions(void)
{
  this->_exitOnEscape = this->_options->chatEscapeCheckBox->checkState();
  this->_typingNotification = this->_options->typingStatusCheckBox->checkState();
  this->_smileys = this->_options->smileysCheckBox->checkState();
  saveCurrentReply();
  emit chatOptionsChanged(this->_exitOnEscape, this->_typingNotification,
			  this->_smileys);
}

QString	OptionsChatWidget::getReply(int index)
{
  switch (index)
    {
    case 0: return this->_replyLocked;
    case 1: return this->_replyAway;
    case 2: return this->_replyServer;
    default:
      {
	std::cerr << "Error: bad index in getReply" << std::endl;
	std::cerr << "Value: " << index << std::endl;
      }
    }
  return "";
}


void	OptionsChatWidget::loadReply(int index)
{
  if (this->_oldComboBoxValue != -42)
    saveCurrentReply(this->_oldComboBoxValue);

  switch (index)
    {
    case 0:
      this->_options->autoReplyLineEdit->setText(this->_replyLocked);
      break;
    case 1:
      this->_options->autoReplyLineEdit->setText(this->_replyAway);
      break;
    case 2:
      this->_options->autoReplyLineEdit->setText(this->_replyServer);
      break;
    default:
      {
	std::cerr << "[Options::loadReply]\n";
	std::cerr << "Error: wrong index.\n";
	std::cerr << "Value: " << index << std::endl;
      }
    }
  this->_replyComboBoxValue = index;
  this->_oldComboBoxValue = index;
}

void	OptionsChatWidget::saveCurrentReply(void)
{
  saveCurrentReply(this->_options->autoReplyComboBox->currentIndex());
}

void	OptionsChatWidget::saveCurrentReply(int index)
{
  switch (index)
    {
    case 0:
      this->_replyLocked = this->_options->autoReplyLineEdit->text();
      break;
    case 1:
      this->_replyAway = this->_options->autoReplyLineEdit->text();
      break;
    case 2:
      this->_replyServer = this->_options->autoReplyLineEdit->text();
      break;
    default:
      {
	std::cerr << "[Options::saveCurrentReply]\n";
	std::cerr << "Error: wrong index.\n";
	std::cerr << "Value: " << index << std::endl;
      }
    }
}
