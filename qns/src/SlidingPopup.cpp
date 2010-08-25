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

#include "SlidingPopup.h"

SlidingPopup::SlidingPopup(const int width, const int height)
  : Popup(width, height)
{
  this->_showAnimationTimer = new QTimer(this);
  this->_hideAnimationTimer = new QTimer(this);
  this->_stopShowingTimer = new QTimer(this);
  this->_stopHidingTimer = new QTimer(this);

  this->_showAnimationTimer->setInterval(10);
  this->_hideAnimationTimer->setInterval(10);
  this->_stopShowingTimer->setInterval((this->_popupHeight + 10) * 10);
  this->_stopHidingTimer->setInterval((this->_popupHeight + 20) * 10);

  this->_stopShowingTimer->setSingleShot(true);
  this->_stopHidingTimer->setSingleShot(true);

  connect(this->_showAnimationTimer, SIGNAL(timeout()), SLOT(moveUp()));
  connect(this->_hideAnimationTimer, SIGNAL(timeout()), SLOT(moveDown()));

  connect(this->_stopShowingTimer, SIGNAL(timeout()),
          this->_showAnimationTimer, SLOT(stop()));
  connect(this->_stopHidingTimer, SIGNAL(timeout()),
          this->_hideAnimationTimer, SLOT(stop()));
}

SlidingPopup::~SlidingPopup()
{
}

void    SlidingPopup::showAnimation(void)
{
  show();
  move(this->_screenWidth - this->_popupWidth, this->_screenHeight);

  this->_showAnimationTimer->start();
  this->_stopShowingTimer->start();
}

void    SlidingPopup::stopAnimation(void)
{
  this->_hiddingTrigger->stop();
  this->_showAnimationTimer->stop();
  this->_hideAnimationTimer->stop();
  this->_stopShowingTimer->stop();
  this->_stopHidingTimer->stop();
  hide();
}

void    SlidingPopup::hideAnimation(void)
{
  this->_hideAnimationTimer->start();
  this->_stopHidingTimer->start();
}

void    SlidingPopup::moveUp(void)
{
  move(x(), y() - 1);
}

void    SlidingPopup::moveDown(void)
{
  move(x(), y() + 1);
}
