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

#ifndef NETWORK_H
#define NETWORK_H

#include <QTimer>
#include <QTcpSocket>
#include <QStringList>

class	Network : public QObject
{
  Q_OBJECT

  public:
  Network(QObject* parent);
  virtual ~Network(void) { this->_socket.close(); }

  QAbstractSocket::SocketState state(void) const { return this->_socket.state(); };
  void	connect(const QString& host, quint16);
  void	disconnect(void);
  void	sendMessage(const char* message) { this->_socket.write(message); }
  void	sendMessage(const QByteArray& message) { this->_socket.write(message); }
  void	resolveLocation(QString& oldLocation) const;

signals:
  void	reconnectionRequest(void);
  void	handShaking(int step, QStringList);
  void	message(const QString& login, const QString& message);
  void	status(const QString& login, const QString& id, const QString& status);
  void	who(const QStringList&);
  void	typingStatus(const QString&, bool);

private slots:
  void	displaySocketError(void);
  void	processPackets(void);

private:
  void	parseLines(void);
  void	interpretLine(const QString& line);

  QString	_rbuffer;
  QTcpSocket	_socket;
  int		_handShakingStep;
  QString	_host;
  quint16	_port;
  QTimer	_reconnectionTimer;
};

#endif // NETWORK_H
