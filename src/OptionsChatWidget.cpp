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
    _notifyTyping(false), _smileys(false),
    _notifyMsg(false), _notifyState(false),
    _oldComboBoxValue(-42), _replyComboBoxValue(0)
{
}

OptionsChatWidget::~OptionsChatWidget(void)
{
}

void    OptionsChatWidget::setOptions(Options* options)
{
  OptionsWidget::setOptions(options);
  connect(this->_options->autoReplyComboBox,
          SIGNAL(currentIndexChanged(int)), SLOT(loadReply(int)));
}

void    OptionsChatWidget::readOptions(QSettings& settings)
{
  settings.beginGroup("ChatOptions");
  this->_exitOnEscape = settings.value("exitonescape", bool(false)).toBool();
  this->_notifyTyping = settings.value("notifytyping", bool(false)).toBool();
  this->_smileys = settings.value("smileys", bool(false)).toBool();
  this->_notifyMsg = settings.value("notifymsg", bool(false)).toBool();
  this->_notifyState = settings.value("notifystate", bool(false)).toBool();
  this->_replyComboBoxValue = settings.value("replycomboboxvalue", int(0)).toInt();
  this->_replyLocked = settings.value("replylocked").toString();
  this->_replyAway = settings.value("replyaway").toString();
  this->_replyServer = settings.value("replyserver").toString();
  settings.endGroup();
}

void    OptionsChatWidget::writeOptions(QSettings& settings)
{
  settings.beginGroup("ChatOptions");
  settings.setValue("exitonescape", this->_exitOnEscape);
  settings.setValue("notifytyping", this->_notifyTyping);
  settings.setValue("smileys", this->_smileys);
  settings.setValue("notifymsg", this->_notifyMsg);
  settings.setValue("notifystate", this->_notifyState);
  settings.setValue("replycomboboxvalue", this->_replyComboBoxValue);
  settings.setValue("replylocked", this->_replyLocked);
  settings.setValue("replyaway", this->_replyAway);
  settings.setValue("replyserver", this->_replyServer);
  settings.endGroup();
}

void    OptionsChatWidget::updateOptions(void)
{
  setCheckState(this->_options->chatEscapeCheckBox, this->_exitOnEscape);
  setCheckState(this->_options->notifyTypingCheckBox, this->_notifyTyping);
  setCheckState(this->_options->smileysCheckBox, this->_smileys);
  setCheckState(this->_options->notifyMsgCheckBox, this->_notifyMsg);
  setCheckState(this->_options->notifyStateCheckBox, this->_notifyState);
  this->_options->autoReplyComboBox->setCurrentIndex(this->_replyComboBoxValue);
  loadReply(this->_replyComboBoxValue);
}

void    OptionsChatWidget::saveOptions(void)
{
  this->_exitOnEscape = this->_options->chatEscapeCheckBox->checkState();
  this->_notifyTyping = this->_options->notifyTypingCheckBox->checkState();
  this->_smileys = this->_options->smileysCheckBox->checkState();
  this->_notifyMsg = this->_options->notifyMsgCheckBox->checkState();
  this->_notifyState = this->_options->notifyStateCheckBox->checkState();
  saveCurrentReply();
}

QString OptionsChatWidget::getReply(const int index) const
{
  switch (index)
    {
    case 1: return this->_replyLocked;
    case 2: return this->_replyAway;
    case 3: return this->_replyServer;
    default:
      qFatal("[OptionsChatWidget::getReply] Error: bad index (%d).", index);
    }
  return "";
}


void    OptionsChatWidget::loadReply(const int index)
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
      qFatal("[OptionsChatWidget::loadReply] Error: bad index (%d).", index);
    }
  this->_replyComboBoxValue = index;
  this->_oldComboBoxValue = index;
}

void    OptionsChatWidget::saveCurrentReply(void)
{
  saveCurrentReply(this->_options->autoReplyComboBox->currentIndex());
}

void    OptionsChatWidget::saveCurrentReply(int index)
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
      qFatal("[OptionsChatWidget::saveCurrentReply] "
	     "Error: bad index (%d).", index);
    }
}
