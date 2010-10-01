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

#ifndef CONTACTS_READER_H_
#define CONTACTS_READER_H_

#include <QIODevice>
#include <QXmlStreamReader>
#include "ContactsTree.h"

class   OptionsWidget;

class ContactsReader : public QXmlStreamReader
{
 public:
  ContactsReader(QTreeWidget *tree, OptionsWidget* options);
  bool read(QIODevice *device);

 private:
  void readUnknownElement(void);
  void readQNS(void);
  void readGroup(QTreeWidgetItem* parent);
  void readContact(QTreeWidgetItem* parent);
  void readBlocked(void);
  inline QTreeWidgetItem* createChildItem(QTreeWidgetItem *item);

 private:
  QTreeWidget*   _tree;
  OptionsWidget* _options;
};

#endif
