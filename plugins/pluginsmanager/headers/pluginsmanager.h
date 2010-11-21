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

#ifndef PLUGINS_MANAGER_H_
#define PLUGINS_MANAGER_H_

#include <QHash>
#include "ui_pluginsmanager.h"

class QMenu;
class Popup;
class QPluginLoader;

class IPopupPlugin;
class IWidgetPlugin;
class IPluginDescriptor;

class PluginsManager : public QWidget, public Ui_PluginsManager
{
  Q_OBJECT

  public:
  PluginsManager(QWidget* parent = 0);
  ~PluginsManager(void);

  void init(QMenu* menu, Popup* popup);
  void loadDefaultDirectory(void);

public slots:
  bool loadPlugin(void);

private:
  template<class Interface>
  bool identifyInterface(QObject* plugin, const QString& path);
  bool isLoaded(const QString& name);
  bool loadPlugin(const QString& path);
  bool identifyPlugin(QObject* plu, QPluginLoader* loa, const QString& path);
  void addToMenu(IPopupPlugin* popupPlugin);
  void addToMenu(IWidgetPlugin* widgetPlugin);
  void populateTable(IPluginDescriptor* descriptor, const QString& path);
  void recursiveSearch(const QString& path);

private:
  QMenu* _menu;
  Popup* _popup;
};

#endif
