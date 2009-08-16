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
#include "ContactsWriter.h"

ContactsWriter::ContactsWriter(const QList<ContactWidget*> list)
 : _contactList(list)
{
}

ContactsWriter::~ContactsWriter(void)
{
}

bool	ContactsWriter::writeFile(QIODevice* device)
{
	setDevice(device);
	writeStartDocument();
	writeDTD("<!DOCTYPE QNetSoul>");
	writeStartElement("QNetSoul");
	writeAttribute("version", "1.0");
	const int size = this->_contactList.size();
	for (int i = 0; i < size; ++i)
		writeContact(this->_contactList[i]);
	writeEndDocument();
	return true;
}

void	ContactsWriter::writeContact(const ContactWidget* contact)
{
	if (contact)
	{
		writeStartElement("contact");
		writeTextElement("login", contact->getLogin());
		writeTextElement("alias", contact->aliasLabel->text());
		writeEndElement();
	}
	else
	{
		std::cerr << "writeContact failed." << std::endl;
	}
}
