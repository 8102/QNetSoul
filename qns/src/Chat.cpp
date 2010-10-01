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

#include <QDateTime>
#include <QScrollBar>
#include <QCloseEvent>
#include <QStatusBar>
#include "Chat.h"
#include "Smileys.h"
#include "Network.h"
#include "OptionsWidget.h"
#include "PortraitResolver.h"

Chat::Chat(const int id, const QString& login, const QString& loc)
  : _id(id), _alias(login), _login(login), _location(loc),
    _network(NULL), _options(NULL)
{
  setupUi(this);
  setPortrait();
  setWindowTitle(login);
  this->loginLabel->setText(login + "@" + loc);
  connect(this->inputTextEdit, SIGNAL(returnPressed()), SLOT(sendMessage()));
  connect(this->inputTextEdit, SIGNAL(textChanged()), SLOT(handleTypingSignal()));
}

Chat::~Chat(void)
{
}

QString Chat::getFormatedDateTime(void) const
{
  return QString('('+QDateTime::currentDateTime().toString("hh:mm:ss")+')');
}

// TODO: add smileys later
void    Chat::insertSmileys(void)
{
  static const Smileys  smileys[] =
    {
      {"=)", ":/images/smile.png"},
      {":)", ":/images/smile.png"},
      {":-)", ":/images/smile.png"},
      {";)", ":/images/wink.png"},
      {";-)", ":/images/wink.png"},
      {"xD", ":/images/evilgrin.png"},
      {"=D", ":/images/grin.png"},
      {":D", ":/images/grin.png"},
      {":-D", ":/images/grin.png"},
      {"^^", ":/images/happy.png"},
      {"=o", ":/images/surprised.png"},
      {":o", ":/images/surprised.png"},
      {":o)", ":/images/surprised.png"},
      {":-o", ":/images/surprised.png"},
      {":-o)", ":/images/surprised.png"},
      {"=p", ":/images/tongue.png"},
      {":p", ":/images/tongue.png"},
      {":-p", ":/images/tongue.png"},
      {"=}", ":/images/waii.png"},
      {":}", ":/images/waii.png"},
      {":(", ":/images/unhappy.png"},
      {"=(", ":/images/unhappy.png"},
      {NULL, NULL}
    };
  bool thereIsSmiley;
  const QTextCursor save = this->outputTextBrowser->textCursor();

  this->outputTextBrowser->moveCursor(QTextCursor::StartOfBlock);
  for (int i = 0; (smileys[i].smiley); ++i)
    {
      do
        {
          thereIsSmiley =
            this->outputTextBrowser->find(smileys[i].smiley,
                                          QTextDocument::FindWholeWords);
          if (true == thereIsSmiley)
            {
              this->outputTextBrowser->textCursor()
                .insertImage(QImage(smileys[i].imagePath));
              this->outputTextBrowser->moveCursor(QTextCursor::StartOfBlock);
            }
        } while (thereIsSmiley);
    }
  this->outputTextBrowser->setTextCursor(save);
}

void    Chat::replaceUrls(QString msg)
{
  //msg = Qt::escape(msg); // magic function <3
  msg.replace(QRegExp("(https?://[a-zA-Z0-9.+%#:_/?=&-]+)"),
              "<a href='\\1'>\\1</a>");
  this->outputTextBrowser->insertHtml(msg);
}

void    Chat::insertMessage(const QString& alias,
                            const QString& msg,
                            const QColor& color)
{
  Q_ASSERT(this->_options);

  int           scrollBarValue = -1;
  QScrollBar*   scrollBar = this->outputTextBrowser->verticalScrollBar();

  if (scrollBar && scrollBar->value() != scrollBar->maximum())
    scrollBarValue = scrollBar->value();
  QString html("<p>");
  html += QString("<span style=' color:%1;'>%2 %3</span>").arg(color.name()).arg(getFormatedDateTime()).arg(alias);
  html.append(": </p>");
  this->outputTextBrowser->moveCursor(QTextCursor::End);
  this->outputTextBrowser->insertHtml(html);
  replaceUrls(msg);
  this->outputTextBrowser->insertHtml("<br />");
  if (this->_options->chatWidget->smileys())
    insertSmileys();
  if (scrollBar)
    {
      if (scrollBarValue != -1)
        scrollBar->setValue(scrollBarValue);
      else
        scrollBar->setValue(scrollBar->maximum());
    }
}

void    Chat::notifyTypingStatus(const bool typing)
{
  if (false == typing)
    setWindowTitle(this->_alias);
  else
    setWindowTitle(this->_alias + tr(" is typing a message..."));
}

void    Chat::setPortrait(void)
{
  QString portraitPath;
  if (PortraitResolver::isAvailable(portraitPath, this->_login))
    {
      setWindowIcon(QIcon(portraitPath));
      this->portraitLabel->setPixmap(QPixmap(portraitPath));
    }
}

void    Chat::autoReply(const int currentStatus)
{
  Q_ASSERT(this->_network);

  // No autoReply if you are online...
  if (currentStatus == 0) return;
  // No autoReply for yourself :)
  if (this->_options->loginLineEdit->text() == this->_login) return;

  const QString autoReplyMsg =
    this->_options->chatWidget->getReply(currentStatus);
  if (autoReplyMsg.isEmpty() == false)
    {
      // Fetch self login
      insertMessage(this->_options->loginLineEdit->text(),
                    autoReplyMsg, QColor(32, 74, 135));
      this->_network->transmitMsg(this->_login,
                                  this->_location,
                                  autoReplyMsg);
    }
}

void    Chat::keyPressEvent(QKeyEvent* event)
{
  Q_ASSERT(this->_options);
  if (Qt::Key_Escape == event->key() &&
      this->_options->chatWidget->exitOnEscape())
    {
      this->hide();
      event->ignore();
    }
}

void    Chat::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  if (!this->_geometry.isNull())
    setGeometry(this->_geometry);
}

void    Chat::hideEvent(QHideEvent* event)
{
  this->_geometry = geometry();
  QWidget::hideEvent(event);
}

void    Chat::closeEvent(QCloseEvent* event)
{
  this->hide();
  event->ignore();
}

void    Chat::sendMessage(void)
{
  Q_ASSERT(this->_network);

  QString message = this->inputTextEdit->toPlainText();
  if (message.length() == 0) return;

#ifndef QT_NO_DEBUG
  qDebug() << "[Chat::sendMessage] Message:" << message;
#endif

  message.replace("\n", "<br />");
  insertMessage(this->_options->loginLineEdit->text(),
                message, QColor(32, 74, 135));
  this->_network->transmitMsg(this->_login, this->_location, message);
  this->inputTextEdit->clear();
}

void    Chat::handleTypingSignal(void)
{
  Q_ASSERT(this->_options);
  Q_ASSERT(this->_network);
  if (this->_options->chatWidget->notifyTyping())
    this->_network->transmitTypingStatus(this->_login, this->_location,
                                         this->inputTextEdit->isEmpty());
}
