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
#include <QImage>
#include <QMessageBox>
#include "Url.h"
#include "PortraitResolver.h"

namespace
{
  const QString	DirName = "portraits";
}

PortraitResolver::PortraitResolver(void)
  : _dir(getPortraitDir()), _http("www.epitech.net")
{
  //setupPortraitDirectory();
  connect(&this->_http, SIGNAL(requestFinished(int, bool)), SLOT(finished(int,bool)));
}

PortraitResolver::~PortraitResolver(void)
{
  this->_requests.clear();
}

void	PortraitResolver::addRequest(const QStringList& logins)
{
  const int size = logins.size();
  for (int i = 0; i < size; ++i)
    addRequest(logins[i], false);
}

void	PortraitResolver::addRequest(const QString& login, bool fun)
{
  if (this->_dir.exists(buildFilename(login, fun)))
    {
      // DEBUG
      //std::cerr << "Portrait already exists." << std::endl;
      //std::cerr << "(" << login.toStdString() << ")\n";
      return;
    }
  else
    {
      // DEBUG
      //std::cerr << "Portrait does not exist." << std::endl;
      //std::cerr << "(" << login.toStdString() << ")\n";
    }
  PortraitRequest* request = new PortraitRequest;

  request->login = login;
  request->fun = fun;
  request->buffer.setBuffer(&request->bytes);
  request->buffer.open(QIODevice::WriteOnly);
  request->id = this->_http.get(QString("/intra/photo.php?fun=%1&login=%2")
				.arg(fun).arg(login), &request->buffer);
  this->_requests.push_back(request);
}

bool	PortraitResolver::isAvailable(QString& portraitPath,
				      const QString& login)
{
  const QDir portraitDir = getPortraitDir();
  const QString fileName = buildFilename(login, false);
  if (portraitDir.exists(fileName))
    {
      portraitPath = portraitDir.dirName() + QDir::separator() + fileName;
      return true;
    }
  return false;
}

QString	PortraitResolver::buildFilename(const QString& login, bool fun)
{
  return fun? (login + "1.jpeg") : (login + "0.jpeg");
}

QDir	PortraitResolver::getPortraitDir(void)
{
  QDir portraitPath(QDir::currentPath());
  if (!portraitPath.exists(DirName))
    portraitPath.mkdir(DirName);
  portraitPath.cd(DirName);
  return portraitPath;
}

void	PortraitResolver::addRequest(const QString& login)
{
  addRequest(login, true);
  addRequest(login, false);
}

void	PortraitResolver::finished(int requestId, bool error)
{
  const int size = this->_requests.size();
  for (int i = 0; i < size; ++i)
    {
      if (this->_requests[i]->id == requestId)
        {
	  if (false == error)
            {
	      QImage img = QImage::fromData(this->_requests[i]->bytes);
	      if (img.save(DirName + QDir::separator() +
			   buildFilename(this->_requests[i]->login,
					 this->_requests[i]->fun)))
		if (this->_requests[i]->fun == false)
		  emit downloadedPortrait(this->_requests[i]->login);
            }
	  else
	    {
	      QMessageBox::critical(NULL, "QNetsoul PortraitResolver",
				    this->_http.errorString());
	    }
	  delete this->_requests.takeAt(i);
	  return;
        }
    }
}

void	PortraitResolver::setupPortraitDirectory(void)
{
  if (!this->_dir.exists(DirName))
    this->_dir.mkdir(DirName);
  this->_dir.cd(DirName);
}
