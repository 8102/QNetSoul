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

#include "LocationResolver.h"

namespace
{
  struct Location
  {
    const QString name;
    const QString ip;
  };
  void	rackLocation(QString& result, const QString& ip)
  {
    Location location[] =
      {
        {"ip20",          "10.246."},
        {"pasteur",       "10.247."},
        {"sr",            "10.248."},
        {"midlab",        "10.249."},
        {"cisco",         "10.250."},
        {"", ""}
      };
    for (int i = 0; !location[i].name.isEmpty(); ++i)
      {
        if (ip.startsWith(location[i].ip))
          {
            result = location[i].name;
            result += "-r" + ip.section('.', 2, 2);
            result += 'p' + ip.section('.', 3, 3);
            return;
          }
      }
  }
  void	smLocation(QString& result, const QString& ip)
  {
    if (result.isEmpty() && ip.startsWith("10.200."))
      result = "sm-" + ip.section('.', 2, 2);
  }
  void	restLocation(QString& result, const QString& ip)
  {
    Location locations[] =
      {
        {"Lab Free",      "10.224.6."},
        {"ESL",           "10.224.5."},
        {"LabTxT",        "10.224.14."},
        {"LabEip",        "10.224.8."},
        {"Lab Astek",     "10.224.1."},
        {"Bocal",         "10.242."},
        {"labtech",       "10.253."},
        {"Wireless",      "10.142."},
        {"lse",           "10.227.42."},
        {"lrde",          "62.139."},
        {"GameDevLab",    "10.226.1."},
        {"lab-scia",      "10.226.2."},
        {"lab-srs",       "10.226.3."},
        {"lab-tel",       "10.226.4."},
        {"lab-mma",       "10.226.5."},
        {"lab-acu",       "10.226.6."},
        {"lab-gistr",     "10.226.7."},
        {"lab-sigl",      "10.226.8."},
        {"Leria",         "10.226.9."},
        {"adm",           "10.1."},
        {"vpn",           "10.10."},
        {"vpn-adm",       "10.11."},
        {"under1",        "10.42.01."},
        {"under2",        "10.42.02."},
        {"under3",        "10.42.03."},
        {"assos",         "10.225."},
        {"s-serv",        "10.42.1."},
        {"senegal",       "172.16."},
        {"adm-ivry",      "172.17.1."},
        {"ivry",          "172.17.142."},
        {"bordeaux",      "172.18.1."},   // adm
        {"bordeaux",      "172.18.142."},
        {"adm-lille",     "172.19.1."},
        {"lille",         "172.19.142."},
        {"adm-lyon",      "172.20.1."},
        {"lyon",          "172.20.142."},
        {"nantes",        "172.21.1."},   // adm
        {"nantes",        "172.21.142."},
        {"strasbourg",    "172.22.1."},   // adm
        {"strasbourg",    "172.22.142."},
        {"toulouse",      "172.23.1."},   // adm
        {"toulouse",      "172.23.142."},
        {"", ""}
      };
    if (result.isEmpty())
      {
        for (int i = 0; !locations[i].name.isEmpty(); i++)
          if (ip.startsWith(locations[i].ip))
            {
              result = locations[i].name;
              return;
            }
        result = "Ext";
      }
  }
}

QString LocationResolver::resolve(const QString& ip)
{
  QString result;
  rackLocation(result, ip);
  smLocation(result, ip);
  restLocation(result, ip);
  return result;
}

QString LocationResolver::resolve(const QHostAddress& hostAddress)
{
  const QString ip = hostAddress.toString();
  return resolve(ip);
}
