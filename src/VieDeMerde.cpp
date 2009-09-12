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
#include "Url.h"
#include "VieDeMerde.h"

VieDeMerde::VieDeMerde(void)
 : _http("www.api.viedemerde.fr")
{
	connect(&this->_http, SIGNAL(requestFinished(int, bool)), SLOT(finished(int,bool)));
	
	this->_buffer.setBuffer(&this->_bytes);
	this->_buffer.open(QIODevice::WriteOnly);
	this->_id = this->_http.get("http://api.viedemerde.fr/1.2/view/random?key=readonly", &this->_buffer);
}

VieDeMerde::~VieDeMerde(void)
{
}

int	VieDeMerde::getALife(void)
{
	return (this->_http.get("http://api.viedemerde.fr/1.2/view/random?key=readonly"));
}

void	VieDeMerde::finished(int requestId, bool error)
{
	if (this->_id == requestId)
	{
		if (false == error)
		{
			QString		buffer(this->_bytes);
			QRegExp		textRegExp("<texte>(.+)</texte>");
			
			if (buffer.contains(textRegExp))
			{
				std::cerr << textRegExp.cap(1).replace("&quot;", "\"").toStdString() << std::endl;
			}
		}
	}
}
