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

#include "RessourcesManager.h"

RessourcesManager*	RessourcesManager::_instance = NULL;

RessourcesManager*	RessourcesManager::getInstance(void)
{
  if (RessourcesManager::_instance == NULL)
    RessourcesManager::_instance = new RessourcesManager();
  return (RessourcesManager::_instance);
}

void	RessourcesManager::deleteInstance(void)
{
  delete RessourcesManager::_instance;
  RessourcesManager::_instance = NULL;
}

const QPixmap&	RessourcesManager::getRessource(const int id) const
{
  if (id >= 0 && id < TOTAL)
    {
      return (*ressources[id]);
    }
  return (*ressources[OFFLINE]);
}

RessourcesManager::RessourcesManager(void)
{
  static const char*	path[] =
    {
      ":/images/offline.png",
      ":/images/online.png",
      ":/images/busy.png",
      ":/images/away.png",
      ":/images/log-in.png",
      ":/images/log-out.png",
      ":/images/chat.png"
    };
  for (int i = 0; i < TOTAL; ++i)
    {
      ressources[i] = new QPixmap(path[i]);
    }
}

RessourcesManager::~RessourcesManager(void)
{
  for (int i = 0; i < TOTAL; ++i)
    {
      delete ressources[i];
    }
}

RessourcesManager::RessourcesManager(const RessourcesManager&)
{
}

RessourcesManager& RessourcesManager::operator=(const RessourcesManager&)
{
  return (*this);
}
