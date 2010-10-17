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

#ifndef TRAY_ICON_H_
#define TRAY_ICON_H_

#include <QTimer>
#include "QNetsoul.h"

class   QMenu;
class   QNetsoul;
class   QSystemTrayIcon;

class   TrayIcon : public QSystemTrayIcon
{
  Q_OBJECT

    public:
  TrayIcon(QNetsoul* parent);
  ~TrayIcon(void);

  void  showMessage(const QString& t, const QString& m, int timeout = 10000);
  void  setEnabledStatusMenu(bool enabled);
  void  startBlinking(void);
  void  stopBlinking(void);

  private slots:
  void  blink(void);
  void  changeStatus(void);

 private:
  void  setupTrayIcon(QNetsoul* parent);

 private:
  QMenu* _statusMenu;
  QTimer _blinkingTimer;

 signals:
  void  moveComboBox(int);
};

#endif
