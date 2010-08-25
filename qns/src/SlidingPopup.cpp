#include <iostream>
#include "QSlidingPopup.h"

QSlidingPopup::QSlidingPopup(const int width, const int height)
  : QPopup(width, height)
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

QSlidingPopup::~QSlidingPopup()
{
}

void	QSlidingPopup::showAnimation(void)
{
  show();
  move(this->_screenWidth - this->_popupWidth, this->_screenHeight);

  this->_showAnimationTimer->start();
  this->_stopShowingTimer->start();
}

void	QSlidingPopup::stopAnimation(void)
{
  this->_hiddingTrigger->stop();
  this->_showAnimationTimer->stop();
  this->_hideAnimationTimer->stop();
  this->_stopShowingTimer->stop();
  this->_stopHidingTimer->stop();
  hide();
}

void	QSlidingPopup::hideAnimation(void)
{
  this->_hideAnimationTimer->start();
  this->_stopHidingTimer->start();
}

void	QSlidingPopup::moveUp(void)
{
  move(x(), y() - 1);
}

void	QSlidingPopup::moveDown(void)
{
  move(x(), y() + 1);
}
