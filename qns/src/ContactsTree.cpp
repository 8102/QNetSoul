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

#include <QDir>
#include <QClipboard>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QApplication>
#include "Options.h"
#include "Network.h"
#include "ContactsTree.h"
#include "ContactsReader.h"
#include "ContactsWriter.h"
#include "ToolTipBuilder.h"
#include "PortraitResolver.h"

namespace
{
  struct State
  {
    const char*           state;
    const char*           pixmap;
    const QString         displayState;
  };
  const State   states[] =
    {
      {"connection",    ":/images/log-in",      QObject::tr("Login")},
      {"logout",        ":/images/offline",     QObject::tr("Offline")},
      {"actif",         ":/images/online",      QObject::tr("Online")},
      {"away",          ":/images/away",        QObject::tr("Away")},
      {"idle",          ":/images/away",        QObject::tr("Idle")},
      {"lock",          ":/images/lock",        QObject::tr("Locked")},
      {"server",        ":/images/server",      QObject::tr("Server")},
      {NULL, NULL, NULL}
    };
  const QString replaceQuestion =
    QObject::tr("<b>A file named \"%1\" already exists.<br />"
                "Do you want to replace it ?</b><br /><br />"
                "The file already exists in \"%2\".<br />"
                "Replacing it will overwrite its contents.");
}

ContactsTree::ContactsTree(QWidget* parent)
  : QTreeWidget(parent), _addContactDialog(this),
    _options(NULL), _network(NULL)
{
  // Setting up some properties
  setAnimated(true);
  setHeaderHidden(true);
  setDefaultDropAction(Qt::MoveAction);
  setDragDropMode(QAbstractItemView::InternalMove);
  connect(&this->_addContactDialog, SIGNAL(newContact(const QStringList&)),
          this, SLOT(addContact(const QStringList&)));
  createContextMenus();
}

ContactsTree::~ContactsTree(void)
{
}

// Add a new group
bool    ContactsTree::addGroup(const QString& groupName)
{
  if (existingGroup(groupName) == true) return false;

  QTreeWidgetItem* root = invisibleRootItem();
  QTreeWidgetItem* group = new QTreeWidgetItem(root);

  // Setting up the new item
  group->setFlags(Qt::ItemIsSelectable  |
                  Qt::ItemIsEditable    |
                  Qt::ItemIsEnabled     |
                  Qt::ItemIsDragEnabled |
                  Qt::ItemIsDropEnabled);
  group->setText(0, groupName);
  group->setData(0, Type, Group);
  group->setData(0, IconPath, ":/images/group.png");
  group->setIcon(0, QIcon(":/images/group.png"));
  Group::buildToolTip(group);

  return true;
}

// properties.at(0): Login
// properties.at(1): Id
// properties.at(2): Ip
// properties.at(3): Promo
// properties.at(4): State
// properties.at(5): Location
// properties.at(6): Comment
bool    ContactsTree::updateConnectionPoint(const QStringList& properties)
{
  Q_ASSERT(properties.size() == 7);
  if (properties.size() != 7)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[ContactsTree::updateConnectionPoint]"
        "Properties must contain 7 fields.";
#endif
      return false;
    }

  // searching contact
  QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildCount = root->childCount();
  int groupChildCount;
  QTreeWidgetItem* group;
  QTreeWidgetItem* contact = NULL;
  for (int i = 0; i < rootChildCount; ++i)
    if (Contact == root->child(i)->data(0, Type).toInt() &&
        properties.at(0) == root->child(i)->data(0, Login))
      {
#ifndef QT_NO_DEBUG
        qDebug() << "[ContactsTree::updateConnectionPoint]"
                 << "Contact found, he has no group";
#endif
        contact = root->child(i);
        break;
      }
    else if (Group == root->child(i)->data(0, Type).toInt())
      {
        group = root->child(i);
        groupChildCount = group->childCount();
        for (int j = 0; j < groupChildCount; ++j)
          if (properties.at(0) == group->child(j)->data(0, Login))
            {
#ifndef QT_NO_DEBUG
              qDebug() << "[ContactsTree::updateConnectionPoint]"
                       << "Contact found, he has a group";
#endif
              contact = group->child(j);
              break;
            }
      }
  if (contact == NULL)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[ContactsTree::updateConnectionPoint]"
               << "Contact not found";
#endif
      return false;
    }

  // searching connection point by unique id
  int cpIndex = 0;
  QTreeWidgetItem* connectionPoint = NULL;
  const int connectionPointsCount = contact->childCount();
  for (; cpIndex < connectionPointsCount; ++cpIndex)
    if (properties.at(1) == contact->child(cpIndex)->data(0, Id).toString())
      {
        connectionPoint = contact->child(cpIndex);
        break;
      }

  // Remove it if State is offline
  if ("logout" == properties.at(4))
    {
      if (connectionPoint != NULL)
        {
          delete contact->takeChild(cpIndex);
          Contact::buildToolTip(contact);
          if (contact->parent() != NULL)
            Group::buildToolTip(contact->parent());
          return true;
        }
#ifndef QT_NO_DEBUG
      else
        qDebug() << "[ContactsTree::updateConnectionPoint]"
                 << "Logout but no connection point...";
#endif
      return false;
    }

  // Create connection point if it does not exist
  if (connectionPoint == NULL)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[ContactsTree::updateConnectionPoint]"
               << "Creating connection point...";
#endif
      connectionPoint = new QTreeWidgetItem(contact);
    }

  // Setting up the new item
  connectionPoint->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  for (int i = 0; (states[i].state); ++i)
    if (states[i].state == properties.at(4))
      {
        connectionPoint->setIcon(0, QIcon(states[i].pixmap));
        connectionPoint->setData(0, State, states[i].displayState);
        break;
      }
  connectionPoint->setText(0, properties.at(5)); // Displaying Location
  connectionPoint->setData(0, Type, ConnectionPoint);
  connectionPoint->setData(0, Login, properties.at(0));
  connectionPoint->setData(0, Id, properties.at(1));
  connectionPoint->setData(0, Ip, properties.at(2));
  connectionPoint->setData(0, Promo, properties.at(3));
  connectionPoint->setData(0, Location, properties.at(5));
  if (properties.at(6) != "")
    connectionPoint->setData(0, Comment, properties.at(6));
  contact->setData(0, Promo, properties.at(3));
  Contact::buildToolTip(contact);
  ConnectionPoint::buildToolTip(connectionPoint);
  if (contact->parent() != NULL)
    Group::buildToolTip(contact->parent());
  return true;
}

void    ContactsTree::removeAllConnectionPoints(void)
{
  const QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildCount = root->childCount();
  int groupChildCount;
  QTreeWidgetItem* group;
  QList<QTreeWidgetItem*> children;

  for (int i = 0; i < rootChildCount; ++i)
    if (Contact == root->child(i)->data(0, Type).toInt())
      {
        children = root->child(i)->takeChildren();
        for (int c = 0; c < children.size(); ++c)
          delete children.at(c);
        Contact::buildToolTip(root->child(i));
      }
    else if (Group == root->child(i)->data(0, Type).toInt())
      {
        group = root->child(i);
        groupChildCount = group->childCount();
        for (int j = 0; j < groupChildCount; ++j)
          {
            children = group->child(j)->takeChildren();
            for (int c = 0; c < children.size(); ++c)
              delete children.at(c);
            Contact::buildToolTip(group->child(j));
          }
        Group::buildToolTip(group);
      }
}

void    ContactsTree::removeGroup(const QString& groupName)
{
  QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildrenCount = root->childCount();

  for (int i = 0; i < rootChildrenCount; ++i)
    if (groupName == root->child(i)->text(0))
      {
        delete root->takeChild(i);
        return;
      }
}

void    ContactsTree::removeContact(const QString& groupName,
                                    const QString& contactName)
{
  const QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildrenCount = root->childCount();
  QTreeWidgetItem* group;
  int groupChildrenCount;

  for (int i = 0; i < rootChildrenCount; ++i)
    if (groupName == root->child(i)->text(0))
      {
        group = root->child(i);
        groupChildrenCount = group->childCount();
        for (int j = 0; j < groupChildrenCount; ++j)
          if (contactName == group->child(j)->text(0))
            {
              delete group->takeChild(j);
              Group::buildToolTip(group);
              return;
            }
      }
}

void    ContactsTree::setPortrait(const QString& login,
                                  const QString& portraitPath)
{
  QTreeWidgetItem* contact;
  if (existingContact(login, &contact) == false)
    {
#ifndef QT_NO_DEBUG
      qDebug() << "[ContactsTree::setPortrait]"
               << login << " does not exist.";
#endif
      return;
    }
  contact->setIcon(0, QIcon(portraitPath));
  contact->setData(0, IconPath, portraitPath);
  Contact::buildToolTip(contact);
}

void    ContactsTree::saveContacts(const QString& fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text))
    {
      QMessageBox::warning(this, "QNetSoul " + tr("Contacts"),
                           tr("Cannot write file %1:\n%2.")
                           .arg(fileName)
                           .arg(file.errorString()));
      return;
    }
  Q_ASSERT(this->_options);
  ContactsWriter writer(this, this->_options);
  writer.writeFile(&file);
}

void    ContactsTree::loadContacts(const QString& fileName)
{
  QFile file(fileName);

  this->clear();
  if (!file.open(QFile::ReadOnly | QFile::Text))
    {
      QMessageBox::warning(this, "QNetSoul " + tr("Contacts"),
                           tr("Cannot read file %1:\n%2.")
                           .arg(fileName)
                           .arg(file.errorString()));
      return;
    }
  Q_ASSERT(this->_options);
  ContactsReader reader(this, this->_options);
  if (!reader.read(&file))
    {
      QMessageBox::warning(this, "QNetSoul " + tr("Contacts"),
                           tr("Parse error in file %1 at line %2,"
                              "column %3:\n%4")
                           .arg(fileName)
                           .arg(reader.lineNumber())
                           .arg(reader.columnNumber())
                           .arg(reader.errorString()));
    }
}

QStringList ContactsTree::getLoginList(void) const
{
  QStringList loginList;
  const QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildCount = root->childCount();
  int groupChildCount;
  QTreeWidgetItem* group;

  for (int i = 0; i < rootChildCount; ++i)
    if (Contact == root->child(i)->data(0, Type).toInt())
      loginList << root->child(i)->data(0, Login).toString();
    else if (Group == root->child(i)->data(0, Type).toInt())
      {
        group = root->child(i);
        groupChildCount = group->childCount();
        for (int j = 0; j < groupChildCount; ++j)
          loginList << group->child(j)->data(0, Login).toString();
      }
  return loginList;
}

QStringList ContactsTree::getGroupList(void) const
{
  QStringList groupList;
  const QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildCount = root->childCount();
  for (int i = 0; i < rootChildCount; ++i)
    if (Group == root->child(i)->data(0, Type).toInt())
      groupList << root->child(i)->text(0);
  return groupList;
}

// Slot used by tree contextMenu
void    ContactsTree::addGroup(void)
{
  bool  ok;
  const QString name =
    QInputDialog::getText(this, tr("Add Group"),
                          tr("Please enter the name of "
                             "the group to be added."),
                          QLineEdit::Normal,
                          "", &ok);
  if (ok && name != "")
    addGroup(name);
}

// Slot used by tree contextMenu
void    ContactsTree::addContact(void)
{
  if (this->_addContactDialog.isVisible() == false)
    {
      this->_addContactDialog.loginLineEdit->clear();
      this->_addContactDialog.loginLineEdit->setFocus();
      this->_addContactDialog.setGroups(getGroupList());
      this->_addContactDialog.loginLineEdit->setFocus();
      this->_addContactDialog.show();
    }
  else this->_addContactDialog.activateWindow();
}

// Slot used by AddContact class
// Add a contact to an existing group if specified
// properties.at(0): group
// properties.at(1): login
// properties.at(2): alias
bool    ContactsTree::addContact(const QStringList& properties)
{
  Q_ASSERT(properties.size() == 3);
  if (existingContact(properties.at(1), NULL) == true) return false;

  QTreeWidgetItem* group = NULL;
  QTreeWidgetItem* contact;
  QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildrenCount = root->childCount();

  if (properties.at(0) != tr("None"))
    for (int i = 0; i < rootChildrenCount; ++i)
      if (properties.at(0) == root->child(i)->text(0))
        group = root->child(i);

  if (group == NULL)
    group = root;

  contact = new QTreeWidgetItem(group);

  // Setting up the new item
  QString portraitPath;

  contact->setFlags(Qt::ItemIsSelectable  |
                    Qt::ItemIsEditable    |
                    Qt::ItemIsEnabled     |
                    Qt::ItemIsDragEnabled);
  contact->setText(0, properties.at(2));
  contact->setData(0, Type, Contact);
  contact->setData(0, Login, properties.at(1));
  contact->setData(0, Promo, tr("Undefined yet"));
  contact->setData(0, IconPath, ":/images/contact.png");
  contact->setIcon(0, QIcon(":/images/contact.png"));
  if (PortraitResolver::isAvailable(portraitPath, properties.at(1)))
    {
      contact->setIcon(0, QIcon(portraitPath));
      contact->setData(0, IconPath, portraitPath);
    }
  else emit downloadPortrait(properties.at(1));
  Contact::buildToolTip(contact);

  // Update group tool tip
  if (group != root)
    Group::buildToolTip(group);

  this->_network->monitorContact(properties.at(1));
  this->_network->refreshContact(properties.at(1));
  return true;
}

// Slot used by tree contextMenu
void    ContactsTree::expandAll(void)
{
  expandGroups();
  expandContacts();
}

// Slot used by tree contextMenu
void    ContactsTree::expandGroups(void)
{
  QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildrenCount = root->childCount();

  for (int i = 0; i < rootChildrenCount; ++i)
    if (Group == root->child(i)->data(0, Type).toInt())
      root->child(i)->setExpanded(true);
}

// Slot used by tree contextMenu
void    ContactsTree::expandContacts(void)
{
  QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildrenCount = root->childCount();
  QTreeWidgetItem* group;
  int groupChildrenCount;

  for (int i = 0; i < rootChildrenCount; ++i)
    if (Contact == root->child(i)->data(0, Type).toInt())
      root->child(i)->setExpanded(true);
    else if (Group == root->child(i)->data(0, Type).toInt())
      {
        group = root->child(i);
        groupChildrenCount = group->childCount();
        for (int j = 0; j < groupChildrenCount; ++j)
          group->child(j)->setExpanded(true);
      }
}

// Slot used by group + contact contextMenu
void    ContactsTree::editCurrentItem(void)
{
  QTreeWidgetItem* item = currentItem();
  if (item)
    editItem(item);
}

// Slot used by group + contact contextMenu
void    ContactsTree::removeCurrentItem(void)
{
  QTreeWidgetItem* item = currentItem();
  if (item == NULL) return;
  if (QMessageBox::Cancel ==
      QMessageBox::question(this, "QNetSoul " + tr("Contacts"),
                            tr("You are about to remove \"%1\" from your"
                               " contact list...<br />"
                               "<b>Are you sure ?</b>")
                            .arg(item->text(0)),
                            QMessageBox::Ok, QMessageBox::Cancel))
    return;
  QTreeWidgetItem* parent = item->parent();
  if (parent == NULL) parent = invisibleRootItem();
  const int index = parent->indexOfChild(item);
  const int type = item->data(0, Type).toInt();
  if (type == Contact || type == Group)
    {
      if (type == Contact)
        emit contactRemoved(item->data(0, Login).toString());
      delete parent->takeChild(index);
      if (type == Contact && parent != invisibleRootItem())
        Group::buildToolTip(parent);
    }
}

// Slot used by group + contact contextMenu
void    ContactsTree::sortContacts(void)
{
  QTreeWidgetItem* item = currentItem();
  if (item == NULL) return;
  item->sortChildren(0, Qt::AscendingOrder);
}

// Slot used by connection point contextMenu
void    ContactsTree::copyIp(void)
{
  QClipboard* clipboard = QApplication::clipboard();
  if (clipboard == NULL) return;
  QTreeWidgetItem* item = currentItem();
  if (item == NULL) return;
  const QString ip = item->data(0, Ip).toString();
  if (ip != "")
    clipboard->setText(ip);
}

// Slot
void    ContactsTree::saveContacts(void)
{
  QString fileName =
    QFileDialog::getSaveFileName(this, tr("Save Contacts File"),
                                 QDir::currentPath(),
                                 "QNetSoul " + tr("Contacts Files (*.qns)"));
  if (fileName.isEmpty())
    return;

  if (!fileName.endsWith(".qns"))
    {
      fileName.append(".qns");
      if (QDir(QDir::currentPath()).exists(fileName))
        {
          QString shortFilename = fileName.section(QDir::separator(), -1);
          QString directory = fileName.section(QDir::separator(), -2, -2);
          if (QMessageBox::Cancel ==
              QMessageBox::question(this, "QNetSoul " + tr("Contacts"),
                                    replaceQuestion
                                    .arg(shortFilename).arg(directory),
                                    QMessageBox::Ok, QMessageBox::Cancel))
            return;
        }
    }
  saveContacts(fileName);
}

void    ContactsTree::loadContacts(void)
{
  if (invisibleRootItem()->childCount() > 0)
    {
      if (QMessageBox::Cancel ==
          QMessageBox::question(this, "QNetSoul " + tr("Contacts"),
                                tr("You are going to erase all of your"
                                   " current contacts...<br />"
                                   "<b>Are you sure ?</b>"),
                                QMessageBox::Ok, QMessageBox::Cancel))
        return;
    }

  const QString fileName =
    QFileDialog::getOpenFileName(this, tr("Load Contacts File"),
                                 QDir::currentPath(),
                                 "QNetSoul " + tr("Contacts Files (*.qns)"));
  if (fileName.isEmpty())
    return;
  loadContacts(fileName);
}

void    ContactsTree::refreshContacts(void)
{
  Q_ASSERT(this->_network);
  this->_network->refreshContacts(getLoginList());
}

void    ContactsTree::monitorContacts(void)
{
  Q_ASSERT(this->_network);
  this->_network->monitorContacts(getLoginList());
}

void    ContactsTree::dropEvent(QDropEvent* event)
{
  QTreeWidgetItem* source = currentItem();
  if (source == NULL) return;

  QTreeWidgetItem* target = itemAt(event->pos());
  QTreeWidgetItem* sourceParent = source->parent();
  const int sourceType = source->data(0, Type).toInt();

  // Group move into another group is forbidden
  if (Group == sourceType && target)
    if (target->data(0, Type).toInt() == Group)
      {
#ifndef QT_NO_DEBUG
        qDebug() << "Forbidden move: group -> group";
#endif
        return;
      }
  const bool expanded = source->isExpanded();
  QTreeWidget::dropEvent(event);
  source->setExpanded(expanded);

  // Update old group if existing
  if (sourceParent && Group == sourceParent->data(0, Type).toInt())
    Group::buildToolTip(sourceParent);
  // Update new group if existing
  if (target && Group == target->data(0, Type).toInt())
    Group::buildToolTip(target);
}

void    ContactsTree::contextMenuEvent(QContextMenuEvent* event)
{
  const QTreeWidgetItem* item = itemAt(event->pos());
  if (item == NULL)
    {
      this->_treeMenu.exec(event->globalPos());
      return;
    }
  switch (item->data(0, Type).toInt())
    {
    case Group:
      {
        this->_sortContacts->setEnabled(item->childCount() > 0 ? true:false);
        this->_groupMenu.exec(event->globalPos());
        break;
      }
    case Contact:
      {
        this->_contactMenu.exec(event->globalPos());
        break;
      }
    case ConnectionPoint:
      {
        this->_connectionPointMenu.exec(event->globalPos());
        break;
      }
    default: qFatal("Unknown type: %d", item->data(0, Type).toInt());
    }
}

// properties.at(0): Login
// properties.at(1): Id
// properties.at(2): Ip
// properties.at(3): Promo
// properties.at(4): State
// properties.at(5): Location
// properties.at(6): Comment
void    ContactsTree::mouseDoubleClickEvent(QMouseEvent* event)
{
  if (Qt::LeftButton != event->button())
    {
      QTreeWidget::mouseDoubleClickEvent(event);
      return;
    }
  QTreeWidgetItem* item = currentItem();
  if (item == NULL) return;
  if (ConnectionPoint == item->data(0, Type).toInt())
    {
      QStringList properties;
      properties << item->data(0, Login).toString()
                 << item->data(0, Id).toString()
                 << item->data(0, Ip).toString()
                 << item->data(0, Promo).toString()
                 << item->data(0, State).toString()
                 << item->data(0, Location).toString()
                 << item->data(0, Comment).toString();
      emit openConversation(properties);
    }
  else
    {
      item->setExpanded(!item->isExpanded());
    }
}

bool    ContactsTree::existingGroup(const QString& name) const
{
  const QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildrenCount = root->childCount();

  for (int i = 0; i < rootChildrenCount; ++i)
    if (name == root->child(i)->text(0) &&
        Group == root->child(i)->data(0, Type))
      return true;
  return false;
}

bool    ContactsTree::existingContact(const QString& login,
                                      QTreeWidgetItem** dst) const
{
  const QTreeWidgetItem* root = invisibleRootItem();
  const int rootChildCount = root->childCount();
  int groupChildCount;
  QTreeWidgetItem* group;

  for (int i = 0; i < rootChildCount; ++i)
    if (Contact == root->child(i)->data(0, Type).toInt() &&
        login == root->child(i)->data(0, Login))
      {
        if (dst) *dst = root->child(i);
        return true;
      }
    else if (Group == root->child(i)->data(0, Type).toInt())
      {
        group = root->child(i);
        groupChildCount = group->childCount();
        for (int j = 0; j < groupChildCount; ++j)
          if (login == group->child(j)->data(0, Login))
            {
              if (dst) *dst = group->child(j);
              return true;
            }
      }
  return false;
}

void    ContactsTree::createContextMenus(void)
{
  // tree menu
  this->_treeMenu.addAction(tr("&Add Group"), this, SLOT(addGroup()));
  this->_treeMenu.addAction(tr("&Add Contact"), this, SLOT(addContact()));
  this->_treeMenu.addSeparator();
  this->_treeMenu.addAction(tr("&Expand All"), this, SLOT(expandAll()));
  this->_treeMenu
    .addAction(tr("&Expand Groups"), this, SLOT(expandGroups()));
  this->_treeMenu
    .addAction(tr("&Expand Contacts"), this, SLOT(expandContacts()));
  this->_treeMenu.addSeparator();
  this->_treeMenu
    .addAction(tr("&Load Contacts"), this, SLOT(loadContacts()));
  this->_treeMenu
    .addAction(tr("&Save Contacts As..."), this, SLOT(saveContacts()));

  // group menu
  this->_groupMenu
    .addAction(tr("&Rename Group"), this, SLOT(editCurrentItem()));
  this->_groupMenu
    .addAction(tr("&Remove Group"), this, SLOT(removeCurrentItem()));
  this->_sortContacts = this->_groupMenu
    .addAction(tr("&Sort Contacts"), this, SLOT(sortContacts()));

  // contact menu
  this->_contactMenu
    .addAction(tr("&Set alias"), this, SLOT(editCurrentItem()));
  this->_contactMenu
    .addAction(tr("&Remove Contact"),this,SLOT(removeCurrentItem()));

  // connection point menu
  this->_connectionPointMenu.addAction(tr("&Copy ip"), this, SLOT(copyIp()));
}
