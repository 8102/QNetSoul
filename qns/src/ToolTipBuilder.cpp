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

#include <QTreeWidgetItem>
#include "ContactsTree.h"
#include "ToolTipBuilder.h"
#include "LocationResolver.h"

namespace Group
{
  void  buildToolTip(QTreeWidgetItem* item)
  {
    Q_ASSERT(item);
    QString tt, offlinePhrase, onlinePhrase;

    // Count offline and online contacts in this group
    int online = 0;
    int offline = 0;
    const int contacts = item->childCount();
    for (int i = 0; i < contacts; ++i)
      if (item->child(i)->childCount())
        ++online;
      else
        ++offline;

    // Build tooltip with those pieces of information
    if (online > 0)
      onlinePhrase = QString("<p>%1 " + QObject::tr("contact") + "%2" +
                             " %3 " + QObject::tr("online in this group ") +
                             "</p>")
        .arg(online)
        .arg(online > 1? QObject::tr("s", "plural") : "")
        .arg(online > 1? QObject::tr("are", "plural") : "is");

    if (offline > 0)
      offlinePhrase = QString("<p>%1 " + QObject::tr("contact") + "%2" +
                              " %3 " + QObject::tr("offline in this group") +
                              "</p>")
        .arg(offline)
        .arg(offline > 1? QObject::tr("s", "plural") : "")
        .arg(offline > 1? QObject::tr("are", "plural") : "is");

    // Concatenate strings
    if (online + offline > 0)
      {
        tt.append(onlinePhrase);
        tt.append(offlinePhrase);
      }
    else
      tt = QObject::tr("This group is empty");
    item->setToolTip(0, tt);
  }
}

namespace Contact
{
  void  buildToolTip(QTreeWidgetItem* item)
  {
    Q_ASSERT(item);
    QString tt;
    tt = QString("<img style=\"float:right;\" src=\"%1\" /> <br />"
                 "<b>" + QObject::tr("Login") + "</b>: %2<br />"
                 "<b>" + QObject::tr("Status") + "</b>: %3<br />"
                 "<b>" + QObject::tr("Group") + "</b>: %4")
      .arg(item->data(0, ContactsTree::IconPath).toString())
      .arg(item->data(0, ContactsTree::Login).toString())
      .arg(item->childCount()?QObject::tr("Online") : QObject::tr("Offline"))
      .arg(item->data(0, ContactsTree::Promo).toString());
    item->setToolTip(0, tt);
  }
}

namespace ConnectionPoint
{
  void  buildToolTip(QTreeWidgetItem* item)
  {
    Q_ASSERT(item);
    QString tt;

    tt = QString("<b>" + QObject::tr("Id") + "</b>: %1<br />"
                 "<b>" + QObject::tr("State") + "</b>: %2<br />"
                 "<b>" + QObject::tr("Ip") + "</b>: %3<br />"
                 "<b>" + QObject::tr("Location") + "</b>: %4 (%5)<br />"
                 "<b>" + QObject::tr("Comment") + "</b>: %6")
      .arg(item->data(0, ContactsTree::Id).toString())
      .arg(item->data(0, ContactsTree::State).toString())
      .arg(item->data(0, ContactsTree::Ip).toString())
      .arg(item->data(0, ContactsTree::Location).toString())
      .arg(LocationResolver::resolve(item->data(0, ContactsTree::Ip).toString()))
      .arg(item->data(0, ContactsTree::Comment).toString());
    item->setToolTip(0, tt);
  }
}
