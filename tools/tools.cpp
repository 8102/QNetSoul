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

#include <QTime>
#include <string>
#include <cstdlib>
#include <QObject>
#include <QString>
#include <algorithm>
#include "tools.h"

// Exported to Updater and InternUpdater
extern const QString OutDir = ".config/QNetSoul/downloads";
extern const QString ServerUrl = "http://qnetsoul.tuxfamily.org/public/";
extern const QString ServerName = "TuxFamily";
extern const QString LastVersionUrl = ServerUrl + "GetLastVersion?platform=";

// Exported to QNetsoul and ContactsTree
extern const State states[] =
  {
    {"connection", ":/images/log-in.png",   QObject::tr("Login")},
    {"logout",     ":/images/offline.png",  QObject::tr("Offline")},
    {"actif",      ":/images/online.png",   QObject::tr("Online")},
    {"away",       ":/images/away.png",     QObject::tr("Away")},
    {"idle",       ":/images/away.png",     QObject::tr("Idle")},
    {"lock",       ":/images/lock.png",     QObject::tr("Locked")},
    {"server",     ":/images/server.png",   QObject::tr("Server")},
    {NULL, NULL, NULL}
  };

namespace
{
  const std::string ascii_table =
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

  const std::string code =
    "}T,f9D2E;SdHt#lY\\3AI|wZ-/K<_`[?o.e$xha*7cq5pzrj!OJy=UL1]R(ui>^'NVm)BF0:Xg@sPQ+{C8kn4vM~Wb\"G&%6";

  // called from rand_n function
  void  init_seed(void)
  {
    static bool init = false;
    if (init)
      {
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        init = true;
      }
  }
}

QString Tools::qnetsoulVersion(void)
{
  return "1.6";
}

QString Tools::defaultComment(void)
{
  return QString("-=[QNetSoul v%1]=-").arg(qnetsoulVersion());
}

// When verbose is true, platform string is gonna be displayed
// When verbose is false, platform string is formatted to be a request
QString Tools::identifyPlatform(const bool verbose)
{
  QString os, architecture, version;

  // Identify Operating System
#if defined(Q_OS_WIN)
  os = verbose? "Windows" : "Win";
#elif defined(Q_OS_LINUX)
  os = verbose? "GNU/Linux" : "Linux";
#elif defined(Q_OS_MAC)
  os = verbose? "Mac OS" : "Mac";
#elif defined(Q_OS_FREEBSD)
  if (verbose == false) return "";
  os = "FreeBSD";
#elif defined(Q_OS_NETBSD)
  if (verbose == false) return "";
  os = "NetBSD";
#elif defined(Q_OS_OPENBSD)
  if (verbose == false) return "";
  os = "OpenBSD";
#elif defined(Q_OS_HURD) // haha
  if (verbose == false) return "";
  os = "GNU Hurd";
#else
  (verbose == false)? return "" : return "Undetected operating system";
#endif

  // Identify Architecture
#if (QT_POINTER_SIZE == 4)
  architecture = verbose? "32 bits" : "32";
#elif (QT_POINTER_SIZE == 8)
  architecture = verbose? "64 bits" : "64";
#else
  if (verbose == false) return "";
  return "Undetected architecture";
#endif

  // Identify Version
#if defined (QT_SHARED) || defined (QT_DLL)
  version = verbose? QObject::tr("Shared") : "shared";
#else
  version = verbose? QObject::tr("Static") : "static";
#endif

  if (verbose)
    return os + " " + architecture + " " + version;
  return os + architecture + version;
}

QString Tools::encrypt(const QString& msg)
{
  std::string std_msg = msg.toStdString();
  const long long int msg_size = std_msg.length();

  for (long long int i = 0; i < msg_size; ++i)
    std_msg[i] = code[std_msg[i] - 33];

  return msg.fromStdString(std_msg);
}

QString Tools::unencrypt(const QString& msg)
{
  std::string std_msg = msg.toStdString();
  const long long int msg_size = std_msg.length();
  const long long int cipher_size = code.length();

  for (long long int i = 0; i < msg_size; ++i)
    for (long long int j = 0; j < cipher_size; ++j)
      if (code[j] == std_msg[i])
        {
          std_msg[i] = ascii_table[j];
          break;
        }
  return msg.fromStdString(std_msg);
}

void Tools::replaceHtmlSpecialChars(QString& text)
{
  text.replace("&amp;", "&");
  text.replace("&#039;", "'");
  text.replace("&quot;", "\"");
  text.replace("&lt;", "<");
  text.replace("&gt;", ">");
}

// Source code imported from developpez.com
// http://c.developpez.com/faq/?page=alea#RANDOM_runif_a_b
int     Tools::rand_n(const int n)
{
  int draw;
  const int partSize   = 1 + (n == RAND_MAX ? 0 : (RAND_MAX - n) / (n + 1));
  const int maxUsefull = partSize * n + (partSize - 1);

  init_seed();
  do
    {
      draw = rand();
    } while (draw > maxUsefull);
  return draw / partSize;
}
