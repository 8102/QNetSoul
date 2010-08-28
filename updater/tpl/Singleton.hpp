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

#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <QObject>

template <class T>
class Singleton
{
public:
  static T& Instance()
  {
    static T _instance; // create static instance of our class
    return _instance;   // return it
  }

private:
  Singleton();// hide constructor
  ~Singleton();// hide destructor
  Singleton(const Singleton &); // hide copy constructor
  Singleton& operator=(const Singleton &); // hide assign op
};


#endif
