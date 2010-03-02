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
#include "State.h"
#include "ContactWidget.h"
#include "LocationResolver.h"
#include "PortraitResolver.h"

enum {LOGIN, LOGOUT, ACTIF, AWAY, LOCK, SERVER, TOTAL};

static const State	states[] =
  {
    {"login", ":/images/log-in", QObject::tr("Login")},
    {"logout", ":/images/offline", QObject::tr("Offline")},
    {"actif", ":/images/online", QObject::tr("Online")},
    {"away", ":/images/away", QObject::tr("Away")},
    {"lock", ":/images/lock", QObject::tr("Locked")},
    {"server", ":/images/server", QObject::tr("Server")},
    {NULL, NULL, NULL}
  };

ContactWidget::ContactWidget(QWidget* parent, const QString& login, const QString& alias)
  : QWidget(parent), _login(login), _state(states[LOGOUT].displayState), _pixmap(states[LOGOUT].pixmap)
{
  setupUi(this);
  if (!alias.isEmpty())
    this->aliasLabel->setText(alias);
  else
    this->aliasLabel->setText(login);

  QDir	portraits("./portraits");
  if (portraits.exists(PortraitResolver::buildFilename(login, false)))
    this->portraitLabel->setPixmap(QPixmap("./portraits/" + PortraitResolver::buildFilename(login, false)));
  buildToolTip();
}

ContactWidget::~ContactWidget(void)
{
}

void	ContactWidget::reset(void)
{
  this->_state = states[LOGOUT].displayState;
  this->_pixmap = states[LOGOUT].pixmap;
  this->statusLabel->setPixmap(QPixmap(states[LOGOUT].pixmap));
  this->_connections.clear();
  buildToolTip();
}

int		ContactWidget::getConnectionsSize(void) const
{
  return (this->_connections.size());
}

QString	ContactWidget::getLogin(void) const
{
  return (this->_login);
}

void	ContactWidget::setLogin(const QString& login)
{
  this->_login = login;
}

bool	ContactWidget::hasGroup(void) const
{
  return (!this->_group.isEmpty());
}

QString	ContactWidget::getGroup(void) const
{
  return (this->_group);
}

void	ContactWidget::setGroup(const QString& group)
{
  this->_group = group;
}

QString	ContactWidget::getStatus(void) const
{
  return (this->_pixmap);
}

void	ContactWidget::addConnectionPoint(const ConnectionPoint& point)
{
  const int	size = this->_connections.size();

  // DEBUG
  std::cerr << "CONNECTIONS SIZE: " << size << std::endl;
  for (int i = 0; i < size; ++i)
    {
      if (this->_connections[i].id == point.id)
        {
	  // DEBUG
	  std::cerr << "STATE IN ADD CONNECTION POINT: ";
	  std::cerr << point.state.toStdString() << std::endl;
	  this->_connections[i] = point;
	  buildToolTip();
	  updateState();
	  std::cerr << "addConnectionPoint Issue FIXME." << std::endl;
	  return;
        }
    }
  this->_connections.push_back(point);
  updateState();
}

void	ContactWidget::updateConnectionPoint(const QString& id,
					     const QString& state)
{
  const int	size = this->_connections.size();

  for (int i = 0; i < size; ++i)
    {
      if (this->_connections[i].id != id)
	continue;
        
      for (int j = 0; (states[j].state); ++j)
	{
	  if (states[j].state == state)
	    {
	      if (states[LOGOUT].state == state)
		{
		  this->_connections.removeAt(i);
		}
	      else
		{
		  // DEBUG
		  std::cerr << "STATE IN UPDATE: " << state.toStdString() << std::endl;
		  this->_connections[i].state = state;
		  //this->_connections[i].state = states[j].displayState;
		}
	      updateState();
	      return;
	    }
	}
      std::cerr << "state non trouve: " << state.toStdString() << std::endl;  
    }
  std::cerr << "Connections size: " << size << std::endl;
}

// A recoder c'est trop laid !
void	ContactWidget::updateState(void)
{
  const int	size = this->_connections.size();

  if (0 == size)
    {
      reset();
    }
  else if (1 == size)
    {
      for (int i = 0; (states[i].state); ++i)
        {
	  if (states[i].state == this->_connections.first().state)
            {
	      // DEBUG
	      std::cerr << "CONTACTWIDGET::UPDATE STATE: ";
	      std::cerr << this->_connections.first().state.toStdString() << std::endl;
	      this->_state = states[i].displayState;
	      this->_pixmap = states[i].pixmap;
	      this->statusLabel->setPixmap(QPixmap(states[i].pixmap));
	      this->update(); // useless ?
	      break;
            }
        }
    }
  else
    {
      int	online = 0;
      int	away = 0;
      int	lock = 0;
      int	server = 0;

      for (int i = 0; i < size; ++i)
        {
	  if (states[ACTIF].state == this->_connections[i].state)
            {
	      ++online;
            }
	  else if (states[AWAY].state == this->_connections[i].state)
            {
	      ++away;
            }
	  else if (states[LOCK].state == this->_connections[i].state)
            {
	      ++lock;
            }
	  else if (states[SERVER].state == this->_connections[i].state)
            {
	      ++server;
            }
        }
      // DEBUG
      //std::cerr << "Online: " << online << std::endl;
      //std::cerr << "Away: " << away << std::endl;
      //std::cerr << "Lock: " << lock << std::endl;
      //std::cerr << "Server: " << server << std::endl;

      if (online >= away && online >= lock && online >= server)
        {
	  this->_state = states[ACTIF].displayState;
	  this->_pixmap = states[ACTIF].pixmap;
	  this->statusLabel->setPixmap(QPixmap(states[ACTIF].pixmap));
        }
      else if (away >= lock && away >= server)
        {
	  this->_state = states[AWAY].displayState;
	  this->_pixmap = states[AWAY].pixmap;
	  this->statusLabel->setPixmap(QPixmap(states[AWAY].pixmap));
        }
      else if (lock > away && lock > server)
        {
	  this->_state = states[LOCK].displayState;
	  this->_pixmap = states[LOCK].pixmap;
	  this->statusLabel->setPixmap(QPixmap(states[LOCK].pixmap));
        }
      else
        {
	  this->_state = states[SERVER].displayState;
	  this->_pixmap = states[SERVER].pixmap;
	  this->statusLabel->setPixmap(QPixmap(states[SERVER].pixmap));
        }
    }
  buildToolTip();
}

void	ContactWidget::buildToolTip(void)
{
  QString	result = "<html><head><meta name='qrichtext' content='1' /><style type='text/css'>";
  result += "p, li { white-space: pre-wrap; }";
  result += "</style></head><body style=\"font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">";
  result += "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
  result += "<img src=\"" + this->_pixmap + "\"/>";
  result += "<span style='font-size:14pt; font-weight:600;'>" + this->_login + "</span></p>";
  result += buildLine("Status:", this->_state);
  if (!this->_group.isEmpty())
    result += buildLine("Group:", this->_group);
  if (this->_connections.size() > 0)
    {
      result += buildLine();
      result += buildConnectionsPointToolTip();
    }
  result += "</body></html>";
  this->setToolTip(result);
}

QString	ContactWidget::buildConnectionsPointToolTip(void) const
{
  const int	size = this->_connections.size();

  QString	result = "<p style=' margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'>";
  result += "<span style=' font-weight:600; text-decoration: underline;'>Connections</span>:</p>";

  for (int i = 0; i < size; ++i)
    {
      QString	location = LocationResolver::resolve(this->_connections[i].ip);

      result += buildLine("State:", this->_connections[i].state);
      if (location.isEmpty())
        {
	  result += buildLine("Ip:", this->_connections[i].ip);
        }
      else
        {
	  location.prepend(this->_connections[i].ip + " (");
	  location.append(')');
	  result += buildLine("Ip:", location);
        }
      result += buildLine("Location:", this->_connections[i].location);
      result += buildLine("Comment:", this->_connections[i].comment);
      if (i + 1 < size)
	result += buildLine();
    }
  return (result);
}

QString	ContactWidget::buildLine(const QString& first, const QString& second) const
{
  QString	result("<p style=' margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'>");
  result += "<span style=' font-weight:600;'>" + first + " </span>" ;
  result += second + "</p>";
  return (result);
}

ContactWidget::ContactWidget(const ContactWidget& copy) : QWidget(copy.parentWidget())
{
}

ContactWidget&	ContactWidget::operator=(const ContactWidget& right)
{
  setParent(right.parentWidget());
  return (*this);
}
