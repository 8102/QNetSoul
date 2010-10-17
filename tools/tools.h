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

#ifndef TOOLS_H_
#define TOOLS_H_

#define QNS_VERBOSE true
#define QNS_RAW     false

class QString;

struct State
{
  const char*   state;
  const char*   pixmap;
  const QString displayState;
};

namespace Tools
{
  QString identifyPlatform(const bool verbose);
  QString encrypt(const QString& msg);
  QString unencrypt(const QString& msg);
  int rand_n(const int n);
}

#endif
