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
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QPluginLoader>

#include "Popup.h"
#include "ipopupplugin.h"
#include "iwidgetplugin.h"
#include "pluginsmanager.h"
#include "iplugindescriptor.h"

namespace
{
  const char* DefaultPluginsDirectory = "plugins";
#if defined(Q_OS_WIN)
  const QString PluginsFilter =
    QObject::tr("Plugins Files") + " *.dll (*.dll)";
#elif defined(Q_OS_MAC)
  const QString PluginsFilter = QObject::tr("Plugins Files") +
    " *.dylib *.bundle *.so (*.dylib *.bundle *.so)";
#elif defined(Q_OS_UNIX) | defined(Q_OS_LINUX)
  const QString PluginsFilter =
    QObject::tr("Plugins Files") + " *.so (*.so)";
#elif defined(Q_OS_HPUX)
  const QString PluginsFilter =
    QObject::tr("Plugins Files") + " *.so *.sl (*.so *.sl)";
#else
  const QString PluginsFilter = tr("All Files") + " (*)";
#endif
}

PluginsManager::PluginsManager(QWidget* parent)
  : QWidget(parent), _menu(NULL), _popup(NULL)
{
  setupUi(this);
  connect(this->loadPushButton, SIGNAL(clicked()), this, SLOT(loadPlugin()));
  QHeaderView* view = this->tableWidget->horizontalHeader();
  if (view)
    view->setSectionResizeMode(QHeaderView::ResizeToContents);
}

PluginsManager::~PluginsManager(void)
{
}

void PluginsManager::init(QMenu* menu, Popup* popup)
{
  this->_menu = menu;
  this->_popup = popup;
}

void PluginsManager::loadDefaultDirectory(void)
{
  Q_ASSERT(this->_menu);
  Q_ASSERT(this->_popup);
  recursiveSearch(DefaultPluginsDirectory);
}

bool PluginsManager::loadPlugin(void)
{
  const QString fileName =
    QFileDialog::getOpenFileName(this, tr("Load QNetSoul Plugin"),
                                 QDir::currentPath(), PluginsFilter);
  if (fileName.isEmpty() == true || QLibrary::isLibrary(fileName) == false)
    return false;
  return loadPlugin(fileName);
}

bool PluginsManager::isLoaded(const QString& name)
{
  const int rowCount = this->tableWidget->rowCount();
  for (int i = 0; i < rowCount; ++i)
    {
      QTableWidgetItem* item = this->tableWidget->item(i, 0);
      if (item && item->text() == name)
        return true;
    }
  return false;
}

bool PluginsManager::loadPlugin(const QString& path)
{
  QPluginLoader* loader = new QPluginLoader(path, this);
  QObject* plugin = loader->instance();
  if (plugin)
    return identifyPlugin(plugin, loader, path);
  return false;
}

template <class Interface>
bool PluginsManager::identifyInterface(QObject* plugin, const QString& path)
{
  Interface* interface = qobject_cast<Interface*>(plugin);
  if (interface != NULL)
    {
      if (isLoaded(interface->name()) == false)
        {
          addToMenu(interface);
          populateTable(interface, path);
          return true;
        }
    }
  return false;
}

bool PluginsManager::identifyPlugin(QObject* plugin,
				    QPluginLoader* loader,
				    const QString& path)
{
  if (identifyInterface<IPopupPlugin>(plugin, path) |
      identifyInterface<IWidgetPlugin>(plugin, path))
    return true;
  delete loader;
  return false;
}

void PluginsManager::addToMenu(IPopupPlugin* popupPlugin)
{
  Q_ASSERT(this->_menu);
  Q_ASSERT(this->_popup);
  this->_menu->addAction(popupPlugin->icon(),
                         popupPlugin->title(),
                         popupPlugin, SLOT(fetchText()),
                         popupPlugin->shortcut());
  connect(popupPlugin, SIGNAL(readyToDisplay(QString, const int)),
          this->_popup, SLOT(showUp(const QString&, const int)));
}

void PluginsManager::addToMenu(IWidgetPlugin* widgetPlugin)
{
  Q_ASSERT(this->_menu);
  this->_menu->addAction(widgetPlugin->icon(),
                         widgetPlugin->title(),
                         widgetPlugin, SLOT(displayWidget()),
                         widgetPlugin->shortcut());
}

void PluginsManager::populateTable(IPluginDescriptor* descriptor,
				   const QString& relativePath)
{
  const int currentRow = this->tableWidget->rowCount();
  this->tableWidget->setRowCount(currentRow + 1);
  QTableWidgetItem* name = new QTableWidgetItem(descriptor->name());
  QTableWidgetItem* version = new QTableWidgetItem(descriptor->version());
  QTableWidgetItem* path = new QTableWidgetItem(relativePath);
  QTableWidgetItem* description = new QTableWidgetItem(descriptor->description());
  this->tableWidget->setItem(currentRow, 0, name);
  this->tableWidget->setItem(currentRow, 1, version);
  this->tableWidget->setItem(currentRow, 2, path);
  this->tableWidget->setItem(currentRow, 3, description);
}

void PluginsManager::recursiveSearch(const QString& path)
{
  QDir dir(path);
  QFileInfoList list = dir.entryInfoList(QDir::Dirs  |
                                         QDir::Files |
                                         QDir::NoDotAndDotDot);
  const int count = list.count();

  for (int i = 0; i < count; ++i)
    {
      QFileInfo info = list[i];
      QString filePath = info.filePath();
      if (info.isDir())
        {
          recursiveSearch(filePath);
        }
      else
        {
          if (QLibrary::isLibrary(filePath))
            {
#ifndef QT_NO_DEBUG
              qDebug() << "[PluginsManager::recursiveSearch]"
                       << "Library found:" << filePath;
#endif
              loadPlugin(filePath);
            }
        }
    }
}
