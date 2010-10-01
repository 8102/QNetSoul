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

#include <QMessageBox>
#include "Url.h"
#include "Network.h"
#include "QNetsoul.h"
#include "OptionsWidget.h"
#include "LocationResolver.h"

Network::Network(QObject* parent)
  : QObject(parent), _options(NULL), _handShakingStep(0)
{
  QNetsoul* ns = dynamic_cast<QNetsoul*>(parent);
  if (ns)
    {
      QObject::connect(this, SIGNAL(reconnectionRequest()),
                       ns, SLOT(reconnect()));
      QObject::connect(&this->_socket, SIGNAL(readyRead()),
                       SLOT(processPackets()));
      QObject::connect(&this->_socket,
                       SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                       ns,
                       SLOT(updateWidgets(QAbstractSocket::SocketState)));
      QObject::connect(&this->_socket,
                       SIGNAL(error(QAbstractSocket::SocketError)),
                       SLOT(handleSocketError()));
    }
  else
    qFatal("Network constructor: parent must be a QNetsoul instance !");
  this->_socket.setProxy(QNetworkProxy::NoProxy);
}

void    Network::connect(const QString& host, quint16 port)
{
  if (QAbstractSocket::UnconnectedState == this->_socket.state())
    {
      this->_host = host;
      this->_port = port;
      this->_socket.connectToHost(host, port);
    }
#ifndef QT_NO_DEBUG
  else
    {
      qDebug() << "[Network::connect] state:"
               << this->_socket.state();
    }
#endif
}

void    Network::disconnect(void)
{
  this->_handShakingStep = 0;
  this->_port = 0;
  this->_host.clear();
  this->_socket.disconnectFromHost();
}

void    Network::resolveLocation(QString& oldLocation) const
{
  if (!oldLocation.isEmpty())
    {
      oldLocation.replace("%L", LocationResolver::resolve(this->_socket.localAddress()));
    }
  else
    {
      oldLocation = LocationResolver::resolve(this->_socket.localAddress());
    }
}

void    Network::refreshContact(const QString& contact)
{
  QByteArray  message("user_cmd who ");
  message.append(contact + '\n');
  sendMessage(message);
}

void    Network::refreshContacts(const QStringList& contacts)
{
#ifndef QT_NO_DEBUG
  qDebug() << "[Network::refreshContacts] refreshing...";
#endif
  const int size = contacts.size();
  if (size == 0) return;

  QByteArray netMsg("user_cmd who {");
  for (int i = 0; i < size; ++i)
    {
      netMsg.append(contacts.at(i));
      if (i + 1 < size)
        netMsg.append(',');
    }
  netMsg.append("}\n");
  sendMessage(netMsg);
}

void    Network::transmitTypingStatus(const QString& login,
                                      const QString& location,
                                      const bool status)
{
  QByteArray typingStatus("user_cmd msg ");
  typingStatus.append("*:" + login);
  typingStatus.append("@*");
  typingStatus.append(url_encode(location.toStdString().c_str()));
  typingStatus.append("*");
  if (!status)
    typingStatus += " dotnetSoul_UserTyping null\n";
  else
    typingStatus += " dotnetSoul_UserCancelledTyping null\n";
  sendMessage(typingStatus);
}

void    Network::transmitMsg(const QString& login,
                             const QString& location,
                             const QString& message)
{
  QByteArray msg("user_cmd msg ");
  msg.append("*:" + login);
  msg.append("@*");
  msg.append(url_encode(location.toStdString().c_str()));
  msg.append("*");
  msg.append(" msg ");
  msg.append(url_encode(message.toStdString().c_str()));
  msg.append('\n');
  sendMessage(msg);
}

void    Network::monitorContact(const QString& contact)
{
#ifndef QT_NO_DEBUG
  qDebug() << "[Network::monitorContact]" << contact;
#endif
  QByteArray msg("user_cmd watch_log_user ");
  msg.append(contact + "\n");
  sendMessage(msg);
}

void    Network::monitorContacts(const QStringList& contacts)
{
#ifndef QT_NO_DEBUG
  qDebug() << "[Network::monitorContacts]" << contacts;
#endif
  const int size = contacts.size();
  if (size == 0) return;

  QByteArray netMsg("user_cmd watch_log_user {");
  for (int i = 0; i < size; ++i)
    {
      netMsg.append(contacts.at(i));
      if (i + 1 < size)
        netMsg.append(',');
    }
  netMsg.append("}\n");
  sendMessage(netMsg);
}

void    Network::sendStatus(const int& status)
{
  switch (status)
    {
    case 0: sendMessage("state actif\n"); break;
    case 1: sendMessage("state lock\n"); break;
    case 2: sendMessage("state away\n"); break;
    case 3: sendMessage("state server\n"); break;
    default: qFatal("[Network::sendStatus] unknown state: %d", status);
    }
}

void    Network::handleSocketError(void)
{
#ifndef QT_NO_DEBUG
  qDebug() << "[Network::handleSocketError]"
           << this->_socket.errorString()
           << "Socket state:" << this->_socket.state();
#endif
  emit reconnectionRequest();
}

void    Network::processPackets(void)
{
  int          readbytes;
  char         buffer[128];
  QDataStream  in(&this->_socket);
  //in.setVersion(QDataStream::Qt_4_6); // Not mandatory...

  while (this->_socket.bytesAvailable())
    {
      readbytes = in.readRawData(buffer, sizeof(buffer) - 1);
      if (readbytes > 0)
        {
          buffer[readbytes] = '\0';
          this->_rbuffer.append(buffer);
        }
      else
        {
          return;
        }
    }
  if (this->_rbuffer.contains('\n'))
    parseLines();
}

void    Network::parseLines(void)
{
  QStringList cmds = this->_rbuffer.split('\n', QString::SkipEmptyParts);

  // Interpret each lines.
  for (int i = 0; i < cmds.size(); ++i)
    interpretLine(cmds[i]);

  // Remove read lines.
  int last = this->_rbuffer.lastIndexOf('\n');
  this->_rbuffer = this->_rbuffer.remove(0, last + 1);
}

void    Network::interpretLine(const QString& line)
{
#ifndef QT_NO_DEBUG
  //qDebug() << line;
#endif
  Q_ASSERT(this->_options);
  QStringList properties;
  QStringList parts = line.split(' ', QString::SkipEmptyParts);
  const int   size = parts.size();

  if (size)
    {
      if ((0 == this->_handShakingStep && "salut" == parts.at(0))         ||
          (1 == this->_handShakingStep && line.startsWith("rep 002 --"))  ||
          (2 == this->_handShakingStep && line.startsWith("rep 002 --")))
        {
          emit handShaking(this->_handShakingStep++, parts);
        }
      else if (line.startsWith("user_cmd") && size >= 4)
        {
          if ("msg" == parts.at(3) && size >= 5)
            {
              const QString message = url_decode(parts.at(4).toStdString().c_str());
              const QString login = parts.at(1).section(':', 3, 3).section('@', 0, 0);
              if (this->_options->blockedWidget->isBlocked(login))
                {
#ifndef QT_NO_DEBUG
                  qDebug() << "[Network::interpretLine]"
                           << "Message blocked from"
                           << login << ":" << message;
#endif
                  return;
                }
#ifndef QT_NO_DEBUG
              qDebug() << "[Network::interpretLine]"
                       << "Message received from"
                       << login << ":" << message;
#endif
              // user_cmd 566:user:1/3:sundas_c@0.0.0.0:~:maison:epitech_2011 | msg test dst=dally_r
              properties << login // login
                         << parts.at(1).section(':', 0, 0) // id
                         << parts.at(1).section(':', 3, 3).section('@', -1) // ip
                         << parts.at(1).section(':', -1) // group
                         << "actif" // state
                         << url_decode(parts.at(1).section(':', -2, -2).toStdString().c_str()) // Location
                         << ""; // Comment
              emit msg(properties, message);
            }
          else if ("state" == parts.at(3) && size >= 5)
            {
              //user_cmd 185:user:1/3:dally_r@0.0.0.0:~:Trolltech%20World:epitech_2011 | state lock
              // properties.at(0): Login
              // properties.at(1): Id
              // properties.at(2): Ip
              // properties.at(3): Promo
              // properties.at(4): State
              // properties.at(5): Location
              // properties.at(6): Comment
              properties << parts.at(1).section(':', 3, 3).section('@', 0, 0) // login
                         << parts.at(1).section(':', 0, 0) // id
                         << parts.at(1).section(':', 3, 3).section('@', -1) // ip
                         << parts.at(1).section(':', -1) // group
                         << parts.at(4).section(':', 0, 0) // state
                         << url_decode(parts.at(1).section(':', -2, -2).toStdString().c_str()) // Location
                         << ""; // Comment
              emit state(properties);
            }
          else if (("login" == parts.at(3) || "logout" == parts.at(3)) && (size >= 4))
            {
              properties << parts.at(1).section(':', 3, 3).section('@', 0, 0) // login
                         << parts.at(1).section(':', 0, 0) // id
                         << parts.at(1).section(':', 3, 3).section('@', -1) // ip
                         << parts.at(1).section(':', -1) // group
                         << parts.at(3) // state
                         << url_decode(parts.at(1).section(':', -2, -2).toStdString().c_str()) // Location
                         << ""; // Comment
              emit state(properties);
            }
          else if ("who" == parts.at(3) && size >= 15)
            {
              // user_cmd 199:user:1/3:dally_r@0.0.0.0:~:maison:epitech_2011
              // | who 329 sundas_c 0.0.0.0 1281146904 1281147024 3 1 ~ maison epitech_2011 actif:1281147031 qnetsoul

              QStringList properties;
              properties << parts.at(5) // login
                         << parts.at(4) // id
                         << parts.at(6) // ip
                         << parts.at(13) // group
                         << parts.at(14).section(':', 0, 0) // state
                         << url_decode(parts.at(12).toStdString().c_str()) // Location
                         << url_decode(parts.at(15).toStdString().c_str()); // Comment
              emit who(properties);
            }
          else if (("dotnetSoul_UserTyping" == parts.at(3) || "dotnetSoul_UserCancelledTyping" == parts.at(3)) && size >= 4)
            {
              // Emits id, bool
              bool ok;
              const int id = parts.at(1).section(':', 0, 0).toInt(&ok);
              if (ok)
                emit typingStatus(id, ("dotnetSoul_UserTyping" == parts.at(3)));
            }
        }
      else if (line.startsWith("ping"))
        {
          sendMessage("ping\n");
#ifndef QT_NO_DEBUG
          qDebug() << "[Network::interpretLine]"
                   << "Ping received, ping answered.";
#endif
        }
      else if (line.startsWith("rep 033 --"))
        {
          emit handShaking(-1, QStringList());
#ifndef QT_NO_DEBUG
          qDebug() << "[Network::interpretLine]"
                   << "Failure...\n"
                   << "Reason:" << line;
#endif
        }
      else
        {
#ifndef QT_NO_DEBUG
          qDebug() << "[Network::interpretLine]"
                   << "Unparsed command:" << line;
#endif
        }
    }
}
