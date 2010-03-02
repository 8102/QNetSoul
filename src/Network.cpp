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

#include <iostream>
#include "Network.h"
#include "QNetsoul.h"
#include "LocationResolver.h"

const quint16	NETSOUL_PORT = 4242;

Network::Network(QObject* parent) : QObject(parent), _handShakingStep(0)
{
  QNetsoul*	ns = dynamic_cast<QNetsoul*>(parent);
  if (ns != NULL)
    {
      QObject::connect(&this->_socket, SIGNAL(readyRead()), SLOT(processPackets()));
      QObject::connect(&this->_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
		       ns, SLOT(updateMenuBar(const QAbstractSocket::SocketState&)));
      QObject::connect(&this->_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
		       ns, SLOT(updateStatusBar(const QAbstractSocket::SocketState&)));
      QObject::connect(&this->_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
		       ns, SLOT(updateStatusComboBox(const QAbstractSocket::SocketState&)));
      QObject::connect(&this->_socket, SIGNAL(error(QAbstractSocket::SocketError)),
		       ns, SLOT(displayError(const QAbstractSocket::SocketError&)));

    }
  else
    {
      std::cerr << "You must give a QNetsoul pointer for the Network's builder" << std::endl;
    }
}

Network::~Network(void)
{
  this->_socket.close();
}

QAbstractSocket::SocketState	Network::state(void) const
{
  return (this->_socket.state());
}

void	Network::connect(const QString& host, quint16 port)
{
  if (QAbstractSocket::UnconnectedState == this->_socket.state())
    {
      this->_socket.connectToHost(host, port);
    }
}

void	Network::disconnect(void)
{
  this->_handShakingStep = 0;
  this->_socket.disconnectFromHost();
}

void	Network::sendMessage(const char* message)
{
  this->_socket.write(message);
}

void	Network::sendMessage(const QByteArray& message)
{
  this->_socket.write(message);
}

void	Network::resolveLocation(QString& oldLocation) const
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

void	Network::processPackets(void)
{
  int			readbytes;
  char			buffer[128];
  QDataStream		in(&this->_socket);
  in.setVersion(QDataStream::Qt_4_5);

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

void	Network::parseLines(void)
{
  QStringList cmds = this->_rbuffer.split('\n', QString::SkipEmptyParts);

  // Interpret each lines.
  for (int i = 0; i < cmds.size(); ++i)
    interpretLine(cmds[i]);

  // Remove read lines.
  int last = this->_rbuffer.lastIndexOf('\n');
  this->_rbuffer = this->_rbuffer.remove(0, last + 1);
}

// A Recoder (refaire en plus propre).
void	Network::interpretLine(const QString& line)
{
  std::cerr << line.toStdString() << std::endl;
  QStringList	parts = line.split(' ', QString::SkipEmptyParts);
  const int	size = parts.size();

  if (size)
    {
      if ((0 == this->_handShakingStep && "salut" == parts.at(0))			||
	  (1 == this->_handShakingStep && line.startsWith("rep 002 --"))	||
	  (2 == this->_handShakingStep && line.startsWith("rep 002 --")))
        {
	  emit handShaking(this->_handShakingStep++, parts);
        }
      else if (line.startsWith("user_cmd") && size >= 4)
        {
	  if ("msg" == parts.at(3) && size >= 5)
            {
	      // Emits login, message
	      emit message(parts.at(1).section(':', 3, 3).section('@', 0, 0), parts.at(4));
            }
	  else if ("state" == parts.at(3) && size >= 5)
            {
	      // Emits login, id, state
	      emit status(parts.at(1).section(':', 3, 3).section('@', 0, 0), parts.at(1).section(':', 0, 0), parts.at(4).section(':', 0, 0));
            }
	  else if (("login" == parts.at(3) || "logout" == parts.at(3)) && (size >= 4))
            {
	      // Emits login, id, state
	      emit status(parts.at(1).section(':', 3, 3).section('@', 0, 0), parts.at(1).section(':', 0, 0), parts.at(3));
            }
	  else if ("who" == parts.at(3) && size > 15)
            {
	      // Emits login(5), group(13), state(14), localisation(12), comment(15)
	      emit who(parts);
            }
	  else if (("dotnetSoul_UserTyping" == parts.at(3) || "dotnetSoul_UserCancelledTyping" == parts.at(3)) && size >= 4)
            {
	      emit typingStatus(parts.at(1).section(':', 3, 3).section('@', 0, 0),
				("dotnetSoul_UserTyping" == parts.at(3)));
            }
        }
      else if (line.startsWith("ping"))
        {
	  sendMessage("ping\n");
        }
      else if (line.startsWith("rep 033 --"))
        {
	  disconnect();
	  std::cerr << "fail..." << std::endl;
	  std::cerr << "Reason: " << line.toStdString() << std::endl;
        }
      else
        {
	  std::cerr << "zOmg quoi !" << std::endl;
	  std::cerr << line.toStdString() << std::endl;
        }
    }
}
