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

#ifndef RESSOURCES_MANAGER_H_
#define RESSOURCES_MANAGER_H_

#include <QMap>
#include <QPixmap>

class	RessourcesManager
{
public:
  static RessourcesManager*	getInstance(void);
  static void			deleteInstance(void);

  enum
    {
      OFFLINE,
      ONLINE,
      AWAY,
      LOGIN,
      LOGOUT,
      CHAT,
      TOTAL
    };

  const QPixmap&	getRessource(const int id) const;

private:
  void loadRessources(void);
  QPixmap* ressources[TOTAL];
  QMap<QString, QPixmap*> avatars;

private:
  static RessourcesManager*	_instance;
  RessourcesManager(void);
  virtual ~RessourcesManager(void);
  RessourcesManager(const RessourcesManager&);
  RessourcesManager& operator=(const RessourcesManager&);
};

#endif // RESSOURCES_MANAGER_H_
