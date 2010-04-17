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
#include <QUrl>
#include <QMimeData>
#include <QClipboard>
#include <QMessageBox>
#include <QApplication>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include "Pastebin.h"

Pastebin::Pastebin(void)
{
  connect(&this->_manager, SIGNAL(finished(QNetworkReply*)),
          SLOT(replyFinished(QNetworkReply*)));
}

Pastebin::~Pastebin(void)
{
}

void	Pastebin::pastebinIt(void)
{
  const QClipboard*	clipboard = QApplication::clipboard();

  //#ifdef Q_WS_X11
  //const QMimeData*	mimeData = clipboard->mimeData(QClipboard::Selection);
  //#else
  const QMimeData*	mimeData = clipboard->mimeData();
  //#endif

  QMessageBox mbox(QMessageBox::Question, "QNetSoul",
		   tr("Are you sure to past this ?"),
		   QMessageBox::Ok | QMessageBox::Cancel);

  if (mimeData == 0)
    return;
  if (mimeData->hasText())
    {
      QString	post_content(mimeData->text());

      mbox.setDetailedText(post_content);
      int ret = mbox.exec();
      if (ret == QMessageBox::Cancel)
	return;

      // DEBUG
      QNetworkProxy	copy = this->_manager.proxy();
      std::cerr << "Proxy type: " << copy.type() << std::endl;

      post_content.prepend("paste_private=1&paste_code=");
      this->_manager.post(QNetworkRequest(QUrl("http://pastebin.com/api_public.php")),
			  post_content.toAscii());
    }
}

void	Pastebin::replyFinished(QNetworkReply* reply)
{
  QByteArray    array = reply->readAll();

  if (!array.startsWith("ERROR"))
    {
      std::cerr << "Reply: " << array.data() << std::endl;
      QString	url = QString("<a href='%1'>%1</a>").arg(array.data());
      QMessageBox::information(0, "PasteBin", url);
    }
  else std::cerr << "Error on pastebin: " << array.data() << std::endl;
  reply->deleteLater();
}
