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

#include <QMenu>
#include <QSystemTrayIcon>
#include "TrayIcon.h"

TrayIcon::TrayIcon(QNetsoul* parent) : QSystemTrayIcon(parent)
{
  setupTrayIcon(parent);
  connect(&this->_blinkingTimer, SIGNAL(timeout()), SLOT(blink()));
  connect(this, SIGNAL(moveComboBox(int)),
          parent->statusComboBox, SLOT(setCurrentIndex(int)));
}

TrayIcon::~TrayIcon(void)
{
}

void    TrayIcon::showMessage(const QString& title,
                              const QString& message,
                              int timeout)
{
  QSystemTrayIcon::showMessage(title, message,
                               QSystemTrayIcon::Information, timeout);
}

void    TrayIcon::setEnabledStatusMenu(bool enabled)
{
  this->_statusMenu->setEnabled(enabled);
}

void    TrayIcon::startBlinking(void)
{
  this->_blinkingTimer.start(1000);
}

void    TrayIcon::stopBlinking(void)
{
  this->_blinkingTimer.stop();
  setIcon(QIcon(":/images/qnetsoul.png"));
}

void TrayIcon::blink(void)
{
  static volatile bool enabled = true;
  if (enabled)
    setIcon(QIcon(":/images/unread.png"));
  else
    setIcon(QIcon());
  enabled = !enabled;
}

void    TrayIcon::changeStatus(void)
{
  QAction* action = dynamic_cast<QAction*>(QObject::sender());
  if (action == NULL) return;
  emit  moveComboBox(action->data().toInt());
}

void    TrayIcon::setupTrayIcon(QNetsoul* parent)
{
  QAction* action;
  QMenu* menu = new QMenu(parent);

  // Vdm
  menu->addAction(QIcon(":/images/vdm.png"), "&Vie de merde",
                  parent->actionVDM, SIGNAL(triggered()));
  menu->addAction(QIcon(":/images/chucknorris.png"),
                  tr("&Chuck Norris facts"),
                  parent->actionCNF, SIGNAL(triggered()));

  // Change status action
  this->_statusMenu = new QMenu(tr("&Change status"), parent);
  this->_statusMenu->setEnabled(false);
  menu->addMenu(this->_statusMenu);

  // Factoriser les 4 actions ?
  action = new QAction(QIcon(":/images/online.png"), tr("&Online"), parent);
  action->setData(QVariant(int(0)));
  this->_statusMenu->addAction(action);
  connect(action, SIGNAL(triggered()), SLOT(changeStatus()));

  action = new QAction(QIcon(":/images/lock.png"), tr("&Locked"), parent);
  action->setData(QVariant(int(1)));
  this->_statusMenu->addAction(action);
  connect(action, SIGNAL(triggered()), SLOT(changeStatus()));

  action = new QAction(QIcon(":/images/away.png"), tr("&Away"), parent);
  action->setData(QVariant(int(2)));
  this->_statusMenu->addAction(action);
  connect(action, SIGNAL(triggered()), SLOT(changeStatus()));

  action = new QAction(QIcon(":/images/server.png"), tr("&Server"), parent);
  action->setData(QVariant(int(3)));
  this->_statusMenu->addAction(action);
  connect(action, SIGNAL(triggered()), SLOT(changeStatus()));

  // Quit action
  menu->addAction(QIcon(":/images/quit.png"), tr("&Quit"),
                  parent, SLOT(saveStateBeforeQuiting()));

  setContextMenu(menu);
  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          parent,
          SLOT(handleClicksOnTrayIcon(QSystemTrayIcon::ActivationReason)));
  setIcon(QIcon(":/images/qnetsoul.png"));
  show();
}
