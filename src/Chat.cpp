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
#include "Chat.h"
#include "PortraitResolver.h"

Chat::Chat(const QString& name, QWidget* parent) : QMainWindow(parent)
{
	setupUi(this);
	setWindowTitle(name);
	this->loginLabel->setText(name);
	QDir	portraits("./portraits");
	if (portraits.exists(PortraitResolver::buildFilename(name, false)))
		this->portraitLabel->setPixmap(QPixmap("./portraits/" + PortraitResolver::buildFilename(name, false)));
	connect(this->inputTextEdit, SIGNAL(returnPressed()), SLOT(sendMessage()));
	//connect(this->inputTextEdit, SIGNAL(textEdited(const QString&)), SLOT(handleTypingSignal(const QString&)));
	connect(actionQuit, SIGNAL(triggered()), SLOT(close()));
}

Chat::~Chat(void)
{
}

void	Chat::insertMessage(const QString& login, const QString& msg, const QColor& color)
{
	bool		keepItOnMax = false;
	QScrollBar*	scrollBar = this->outputTextEdit->verticalScrollBar();

	if (scrollBar)
		keepItOnMax = (scrollBar->value() == scrollBar->maximum());
	this->outputTextEdit->setTextColor(color);
	this->outputTextEdit->insertPlainText(QString("(" + QDateTime::currentDateTime().toString("h:m:s") + ") "));
	this->outputTextEdit->insertPlainText(QString(login + ":"));
	this->outputTextEdit->setTextColor(QColor(0, 0, 0));
	this->outputTextEdit->insertPlainText(QString(" " + msg + '\n'));
	if (scrollBar && keepItOnMax)
		scrollBar->setValue(scrollBar->maximum());
}

void	Chat::notifyTypingStatus(const QString& login, bool typing)
{
	if (false == typing)
		this->statusBar()->clearMessage();
	else
		this->statusBar()->showMessage(QString(login + tr(" is currently typing a message...")));
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
	const int		length = message.length();
	
	if (length > 0)
	{
		if (length < 150)
		{
			emit msgToSend(windowTitle(), message);
			this->inputTextEdit->clear();
		}
		else
		{
			emit msgToSend(windowTitle(), message.left(150));
			this->inputTextEdit->setPlainText(message.right(length - 150));
		}
	}
}

void	Chat::handleTypingSignal(const QString& text)
{
	if (text.isEmpty())
	{
		emit typingSignal(windowTitle(), true);
	}
	else
	{
		emit typingSignal(windowTitle(), false);
	}
}
