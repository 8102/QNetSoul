/*
  Copyright 2009 Dally Richard
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

#ifndef LOCATION_RESOLVER_H
#define LOCATION_RESOLVER_H

#include <QString>
#include <QHostAddress>

class	LocationResolver
{
 public:
  static QString	resolve(const QString& ip);
  static QString	resolve(const QHostAddress& hostAddress);

 private:
  static void		rackLocation(QString& result, const QString& ip);
  static void		smLocation(QString& result, const QString& ip);
  static void		restLocation(QString& result, const QString& ip);
};

struct	Location
{
  const QString	name;
  const QString	ip;
};

#endif // LOCATION_RESOLVER_H
