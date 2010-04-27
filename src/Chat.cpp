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
#include <QDateTime>
#include <QScrollBar>
#include <QCloseEvent>
#include <QStatusBar>
#include "Chat.h"
#include "Smileys.h"
#include "PortraitResolver.h"

Chat::Chat(const QString& name, bool exitOnEscape, bool smileys)
  : _exitOnEscape(exitOnEscape), _smileys(smileys)
{
  setupUi(this);
  setWindowTitle(name);
  this->loginLabel->setText(name);
  QDir	portraits("./portraits");
  if (portraits.exists(PortraitResolver::buildFilename(name, false)))
    {
      const QString	path = "./portraits/" + PortraitResolver::buildFilename(name, false);

      setWindowIcon(QIcon(path));
      this->portraitLabel->setPixmap(QPixmap(path));
    }
  connect(this->inputTextEdit, SIGNAL(returnPressed()), SLOT(sendMessage()));
  connect(this->inputTextEdit, SIGNAL(textChanged()), SLOT(handleTypingSignal()));
}

Chat::~Chat(void)
{
}

QString	Chat::getFormatedDateTime(void) const
{
  return (QString('(' + QDateTime::currentDateTime().toString("h:m:s") + ')'));
}

void	Chat::insertSmileys(void)
{
  static const Smileys	smileys[] =
    {
      {":)", ":/images/smile.png"},
      {";)", ":/images/wink.png"},
      {"xD", ":/images/evilgrin.png"},
      {":D", ":/images/grin.png"},
      {"^^", ":/images/happy.png"},
      {":o", ":/images/surprised.png"},
      {":p", ":/images/tongue.png"},
      {":}", ":/images/waii.png"},
      {":(", ":/images/unhappy.png"},
      {NULL, NULL}
    };
  bool			thereIsSmiley;
  const QTextCursor	save = this->outputTextBrowser->textCursor();

  this->outputTextBrowser->moveCursor(QTextCursor::StartOfBlock);
  for (int i = 0; (NULL != smileys[i].smiley); ++i)
    {
      do
        {
	  thereIsSmiley = this->outputTextBrowser->find(smileys[i].smiley, QTextDocument::FindWholeWords);
	  if (true == thereIsSmiley)
            {
	      this->outputTextBrowser->textCursor().insertImage(QImage(smileys[i].imagePath));
	      this->outputTextBrowser->moveCursor(QTextCursor::StartOfBlock);
            }
        } while (thereIsSmiley);
    }
  this->outputTextBrowser->setTextCursor(save);
}

// Version 0.04 replaced url + html entities "< >"
void	Chat::replaceUrls(QString msg)
{
  msg = Qt::escape(msg); // magic function <3
  msg.replace(QRegExp("(https?://[a-zA-Z0-9.:_/?=&-]+)"), "<a href='\\1'>\\1</a>");
  this->outputTextBrowser->insertHtml(msg);
}

void	Chat::insertMessage(const QString& login, const QString& msg, const QColor& color)
{
  int		scrollBarValue = -1;
  QScrollBar*	scrollBar = this->outputTextBrowser->verticalScrollBar();

  if (scrollBar && scrollBar->value() != scrollBar->maximum())
    scrollBarValue = scrollBar->value();
  QString	html("<p>");
  html += QString("<span style=' color:%1;'>%2 %3</span>").arg(color.name()).arg(getFormatedDateTime()).arg(login);
  html.append(": </p>");
  this->outputTextBrowser->moveCursor(QTextCursor::End);
  this->outputTextBrowser->insertHtml(html);
  replaceUrls(msg);
  this->outputTextBrowser->insertHtml("<br />");
  if (this->_smileys)
    insertSmileys();

  if (scrollBar)
    {
      if (scrollBarValue != -1)
	scrollBar->setValue(scrollBarValue);
      else
	scrollBar->setValue(scrollBar->maximum());
    }
}

void	Chat::notifyTypingStatus(const QString& login, bool typing)
{
  if (false == typing)
    setWindowTitle(login);
  else
    setWindowTitle(QString(login + tr(" is typing a message...")));
}

void	Chat::keyPressEvent(QKeyEvent* event)
{
  if (Qt::Key_Escape == event->key() && this->_exitOnEscape)
    {
      this->hide();
      event->ignore();
    }
  else
    {
      QWidget::keyPressEvent(event);
    }
}

void	Chat::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  if (!this->_geometry.isNull())
    setGeometry(this->_geometry);
}

void	Chat::closeEvent(QCloseEvent* event)
{
  this->_geometry = geometry();
  this->hide();
  event->ignore();
}

void	Chat::sendMessage(void)
{
  const QString	message = this->inputTextEdit->toPlainText();
  const int	length = message.length();

  if (length > 0)
    {
      if (length < 512)
        {
	  emit msgToSend(this->loginLabel->text(), message);
	  this->inputTextEdit->clear();
        }
      else
        {
	  emit msgToSend(this->loginLabel->text(), message.left(256));
	  this->inputTextEdit->setPlainText(message.right(length - 256));
        }
    }
}

void	Chat::handleTypingSignal(void)
{
  emit typingSignal(this->loginLabel->text(),
		    this->inputTextEdit->toPlainText().isEmpty());
}
