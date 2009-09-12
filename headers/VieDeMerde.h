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

#ifndef VIEDEMERDE_H
#define VIEDEMERDE_H

#include <QHttp>
#include <QBuffer>

class	VieDeMerde : public QObject
{
	Q_OBJECT

public:
	VieDeMerde(void);
	virtual ~VieDeMerde(void);

	int		getALife(void);

signals:
	void	vdmReceived(const int& id, const QString& content);

private slots:
	void	finished(int requestId, bool error);

private:
	int			_id;
	QBuffer		_buffer;
	QByteArray	_bytes;
	QHttp		_http;
};

#endif // VIEDEMERDE_H
