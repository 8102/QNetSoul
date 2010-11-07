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

#ifndef CONTACTS_TREE_H_
#define CONTACTS_TREE_H_

#include <QMenu>
#include <QString>
#include <QDropEvent>
#include <QTreeWidget>
#include <QContextMenuEvent>
#include "AddContact.h"

class   Network;
class   OptionsWidget;

class   ContactsTree : public QTreeWidget
{
  Q_OBJECT

  public:
  enum ItemRole { Type = Qt::UserRole,
                  Login, Id, Ip, Promo, State,
                  Location, Comment, IconPath, Fun };
  enum ItemType { Group, Contact, ConnectionPoint };

  ContactsTree(QWidget* parent = NULL);
  ~ContactsTree(void);

  void  setOptions(OptionsWidget* options) { this->_options = options; }
  void  setNetwork(Network* network) { this->_network = network; }

  void  initTree(void);
  bool  addGroup(const QString& groupName);
  bool  updateConnectionPoint(const QStringList& properties);
  void  removeAllConnectionPoints(void);
  void  removeGroup(const QString& groupName);
  void  removeContact(const QString& groupName, const QString& contactName);
  void  setPortrait(const QString& login, const QString& portraitPath);
  void  saveContacts(const QString& fileName);
  bool  loadContacts(const QString& fileName);
  QStringList getLoginList(void) const;
  QStringList getGroupList(void) const;
  QString getAliasByLogin(const QString& login) const;

public slots:
  void  addGroup(void);
  void  addContact(void);
  bool  addContact(const QStringList& properties);
  void  expandAll(void);
  void  expandGroups(void);
  void  expandContacts(void);
  void  editCurrentItem(void);
  void  removeCurrentItem(void);
  void  sortContacts(void);
  void  togglePortrait(void);
  void  copyIp(void);
  void  saveContacts(void);
  void  saveContactsAs(void);
  void  loadContacts(void);
  void  refreshContacts(void);
  void  monitorContacts(void);

signals:
  void  downloadPortrait(const QString& login, bool fun);
  void  openConversation(const QStringList&);
  void  contactRemoved(const QString& login);

protected:
  virtual void dropEvent(QDropEvent* event);
  virtual void contextMenuEvent(QContextMenuEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
  bool  existingGroup(const QString& name) const;
  bool  existingContact(const QString& login, QTreeWidgetItem** dst) const;
  void  openConversation(QTreeWidgetItem* connectionPoint);
  void  togglePortrait(QTreeWidgetItem* contact);
  void  createContextMenus(void);

private:
  QMenu          _treeMenu;
  QMenu          _groupMenu;
  QAction*       _sortContacts;
  QAction*       _portraitType;
  QMenu          _contactMenu;
  QMenu          _connectionPointMenu;
  AddContact     _addContactDialog;
  Network*       _network;
  OptionsWidget* _options;
};

#endif
