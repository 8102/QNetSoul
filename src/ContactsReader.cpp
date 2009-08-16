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

#include "ContactsReader.h"

ContactsReader::ContactsReader(void)
{
}

ContactsReader::~ContactsReader(void)
{
}

QList<Contact>	ContactsReader::read(QIODevice* device)
{
	QList<Contact>	list;
	
	setDevice(device);
	while (!atEnd())
	{
		readNext();
		if (isStartElement())
		{
			if (name() == "QNetSoul" && attributes().value("version") == "1.0")
			{
				readContacts(list);
			}
			else
			{
				raiseError(QObject::tr("The file is not a QNetSoul's contact list."));
			}
		}
	}
	return (list);
}

void	ContactsReader::readContacts(QList<Contact>& list)
{
	while (!atEnd())
	{
		Contact	contact;
		readContact(contact);
		list.push_back(contact);
	}
}

void	ContactsReader::readContact(Contact& contact)
{
	while (!atEnd())
	{
		readNext();
		if (isEndElement())
			break;
		if (isStartElement())
		{
			if (name() == "login")
				contact.login = readElementText();
			else if (name() == "alias")
				contact.alias = readElementText();
		}
	}
}
