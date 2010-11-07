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

#include "OptionsWidget.h"
#include "ContactsReader.h"
#include "ToolTipBuilder.h"
#include "PortraitResolver.h"

namespace
{
  const char* errVersion = "The file is not a qns version 1.0 file.";
}

ContactsReader::ContactsReader(QTreeWidget* tree, OptionsWidget* options)
  : _tree(tree), _options(options)
{
}

bool    ContactsReader::read(QIODevice *device)
{
  setDevice(device);

  while (!atEnd())
    {
      readNext();
      if (isStartElement())
        {
          if (name() == "qns" && attributes().value("version") == "1.0")
            readQNS();
          else
            raiseError(QObject::tr(errVersion));
        }
    }
  return !error();
}

void    ContactsReader::readUnknownElement(void)
{
  Q_ASSERT(isStartElement());

  while (!atEnd())
    {
      readNext();

      if (isEndElement())
        break;

      if (isStartElement())
        readUnknownElement();
    }
}

void    ContactsReader::readQNS(void)
{
  Q_ASSERT(isStartElement() && name() == "qns");

  while (!atEnd())
    {
      readNext();

      if (isEndElement())
        break;

      if (isStartElement())
        {
          if (name() == "Group")
            readGroup(0);
          else if (name() == "Contact")
            readContact(0);
          else if (name() == "BlockedContact")
            readBlocked();
          else
            readUnknownElement();
        }
    }
}

void    ContactsReader::readGroup(QTreeWidgetItem* parent)
{
  Q_ASSERT(isStartElement() && name() == "Group");

  QTreeWidgetItem* group = createChildItem(parent);
  const bool expanded = (attributes().value("expanded") == "yes");

  this->_tree->setItemExpanded(group, expanded);
  group->setIcon(0, QIcon(":/images/group.png"));
  group->setData(0, ContactsTree::Type, ContactsTree::Group);
  group->setData(0, ContactsTree::IconPath, ":/images/group.png");
  group->setFlags(Qt::ItemIsSelectable  |
                  Qt::ItemIsEditable    |
                  Qt::ItemIsEnabled     |
                  Qt::ItemIsDragEnabled |
                  Qt::ItemIsDropEnabled);
  while (!atEnd())
    {
      readNext();

      if (isEndElement())
        break;

      if (name() == "name")
        group->setText(0, readElementText());

      if (isStartElement())
        {
          if (name() == "Contact")
            readContact(group);
          else
            readUnknownElement();
        }
    }
  Group::buildToolTip(group);
}

void    ContactsReader::readContact(QTreeWidgetItem* parent)
{
  Q_ASSERT(isStartElement() && name() == "Contact");

  QString login, portraitPath, fun;
  QTreeWidgetItem* contact = createChildItem(parent);
  const bool expanded = (attributes().value("expanded") == "yes");

  this->_tree->setItemExpanded(contact, expanded);
  contact->setIcon(0, QIcon(":/images/contact.png"));
  contact->setData(0, ContactsTree::Type, ContactsTree::Contact);
  contact->setData(0, ContactsTree::IconPath, ":/images/contact.png");
  contact->setData(0, ContactsTree::Fun, QNS_NORMAL);
  contact->setFlags(Qt::ItemIsSelectable  |
                    Qt::ItemIsEditable    |
                    Qt::ItemIsEnabled     |
                    Qt::ItemIsDragEnabled);
  while (!atEnd())
    {
      readNext();

      if (isEndElement())
        break;

      if (isStartElement())
        {
          if (name() == "alias")
            contact->setText(0, readElementText());
          else if (name() == "login")
            {
              login = readElementText();
              contact->setData(0, ContactsTree::Login, login);
            }
          else if (name() == "promo")
            contact->setData(0, ContactsTree::Promo, readElementText());
          else if (name() == "fun")
            {
              fun = readElementText();
              contact->setData(0, ContactsTree::Fun, fun);
            }
          else readUnknownElement();
        }
    }
  if (login.isEmpty())
    {
#ifndef QT_NO_DEBUG
      parent->removeChild(contact);
      delete contact;
      qDebug() << "[ContactsReader::readContact]"
               << "A contact does not have a login";
#endif
      return;
    }
  if (PortraitResolver::isAvailable(portraitPath, login, fun == "true"))
    {
      contact->setIcon(0, QIcon(portraitPath));
      contact->setData(0, ContactsTree::IconPath, portraitPath);
    }
  Contact::buildToolTip(contact);
}

void    ContactsReader::readBlocked(void)
{
  while (!atEnd())
    {
      readNext();
      if (isEndElement())
        break;
      if (isStartElement())
        {
          if (name() == "login")
            this->_options->blockedWidget->addBlockedContact(readElementText());
          else readUnknownElement();
        }
    }
}

QTreeWidgetItem* ContactsReader::createChildItem(QTreeWidgetItem *item)
{
  QTreeWidgetItem* childItem;

  if (item)
    {
      childItem = new QTreeWidgetItem(item);
    }
  else
    {
      childItem = new QTreeWidgetItem(this->_tree);
    }
  return childItem;
}
