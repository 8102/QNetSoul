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

#include <string>
#include <iostream>
#include <algorithm>
#include "Encryption.h"

namespace
{
  const std::string ascii_table =
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

  const std::string code =
    "}T,f9D2E;SdHt#lY\\3AI|wZ-/K<_`[?o.e$xha*7cq5pzrj!OJy=UL1]R(ui>^'NVm)BF0:Xg@sPQ+{C8kn4vM~Wb\"G&%6";
}

// tester avec "

//void  encryption_init(void) __attribute__ ((constructor));

/*
  void  encryption_init(void)
  {
  //random_shuffle(code.begin(), code.end());
  qDebug() << code;
  //return ;
  }
*/

QString         encrypt(const QString& msg)
{
  std::string   std_msg = msg.toStdString();
  const int     size = std_msg.length();

  for (int i = 0; i < size; ++i)
    std_msg[i] = code[std_msg[i] - 33];

  return msg.fromStdString(std_msg);
}

QString         unencrypt(const QString& msg)
{
  std::string   std_msg = msg.toStdString();
  const int     size = std_msg.length();

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < 94; ++j)
      if (code[j] == std_msg[i])
        {
          std_msg[i] = ascii_table[j];
          break;
        }
  return msg.fromStdString(std_msg);
}
