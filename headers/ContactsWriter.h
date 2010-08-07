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

#ifndef CONTACTS_WRITER_H_
#define CONTACTS_WRITER_H_

#include <QIODevice>
#include <QXmlStreamWriter>
#include "ContactsTree.h"

class   ContactsWriter : public QXmlStreamWriter
{
 public:
  ContactsWriter(QTreeWidget* tree);
  bool  writeFile(QIODevice* device);

 private:
  void writeItem(QTreeWidgetItem *item);

 private:
  QTreeWidget* _tree;
};

#endif
