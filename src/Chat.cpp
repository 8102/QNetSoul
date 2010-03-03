/*
  Copyright 2009 Dally Richard
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

Chat::Chat(const QString& name)
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

QString	Chat::replaceUrls(const QString& input)
{
  QString	result(input);

  //result.replace(QRegExp("(http://[a-z0-9._/-]+)"), "<a href='\\1'>\\1</a>");
  return (result);
}

void	Chat::insertMessage(const QString& login, const QString& msg, const QColor& color)
{
  int		scrollBarValue = -1;
  QScrollBar*	scrollBar = this->outputTextBrowser->verticalScrollBar();

  std::cerr << "DEBUG INSERT MESSAGE: " << msg.toStdString() << std::endl;
  if (scrollBar && scrollBar->value() != scrollBar->maximum())
    scrollBarValue = scrollBar->value();
  QString	html("<p>");
  html += QString("<span style=' color:%1;'>%2 %3</span>").arg(color.name()).arg(getFormatedDateTime()).arg(login);
  html.append(": </p>");
  this->outputTextBrowser->moveCursor(QTextCursor::End);
  this->outputTextBrowser->insertHtml(html);
  // TODO: Interpret lines (url etc)
  this->outputTextBrowser->insertPlainText(msg);
  this->outputTextBrowser->insertHtml("<br />");
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
    setWindowTitle(QString(login + tr(" is currently typing a message...")));
}

void	Chat::keyPressEvent(QKeyEvent* event)
{
  if (Qt::Key_Escape == event->key())
    {
      this->hide();
      event->ignore();
    }
  else
    {
      QWidget::keyPressEvent(event);
    }
}

void	Chat::closeEvent(QCloseEvent* event)
{
  this->hide();
  event->ignore();
}

void	Chat::sendMessage(void)
{
  const QString	message = this->inputTextEdit->toPlainText();
  const int	length = message.length();

  if (length > 0)
    {
      if (length < 300)
        {
	  emit msgToSend(this->loginLabel->text(), message);
	  this->inputTextEdit->clear();
        }
      else
        {
	  emit msgToSend(this->loginLabel->text(), message.left(150));
	  this->inputTextEdit->setPlainText(message.right(length - 150));
        }
    }
}

void	Chat::handleTypingSignal(void)
{
  if (this->inputTextEdit->toPlainText().isEmpty())
    {
      emit typingSignal(this->loginLabel->text(), true);
    }
  else
    {
      emit typingSignal(this->loginLabel->text(), false);
    }
}
