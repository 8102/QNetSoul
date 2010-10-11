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

#include <QPainter>
#include "Popup.h"

namespace
{
  qreal INITIAL_OPACITY = 0.9;
  qreal HOVER_OPACITY = 0.2;
}

Popup::Popup(const int width, const int height)
  : _popupWidth(width), _popupHeight(height),
    _textColor("#CC6633"), _backgroundColor("#171722"),
    _frameColor("#B1B1CB"), _cache(NULL)
{
  init();
  this->_hiddingTrigger = new QTimer(this);
  this->_hiddingTrigger->setSingleShot(true);
  connect(this->_hiddingTrigger, SIGNAL(timeout()), SLOT(hideAnimation()));
}

Popup::~Popup(void)
{
}

void    Popup::showUp(const QString& text, const int timeout)
{
  if (isVisible()) stopAnimation();

  setWindowOpacity(INITIAL_OPACITY);
  this->_text.setText("<font color=" + this->_textColor.name() + '>' +
                      text + "</font>");
  this->_hiddingTrigger->setInterval(timeout);
  this->_hiddingTrigger->start();
  showAnimation();
}

void    Popup::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);

  if (this->_cache == NULL)
    {
      buildCache();
    }
  else
    {
      QPainter painter(this);
      painter.drawPixmap(0, 0, *this->_cache);
    }
}

void    Popup::mousePressEvent(QMouseEvent* event)
{
  Q_UNUSED(event);
  stopAnimation();
}

void    Popup::mouseMoveEvent(QMouseEvent* event)
{
  if (normalGeometry().contains(mapToGlobal(event->pos()), true))
    {
      setWindowOpacity(HOVER_OPACITY);
    }
  else
    {
      setWindowOpacity(INITIAL_OPACITY);
    }
}

void    Popup::showAnimation(void)
{
  show();
}

void    Popup::stopAnimation(void)
{
  this->_hiddingTrigger->stop();
  hide();
}

void    Popup::hideAnimation(void)
{
  hide();
}

void    Popup::init(void)
{
  const QRect geo = QApplication::desktop()->screenGeometry(this);
  this->_screenWidth = geo.width();
  this->_screenHeight = geo.height();

  this->_text.setFrameShape(QFrame::NoFrame);
  this->_text.setAttribute(Qt::WA_TransparentForMouseEvents);
  this->_text.setWordWrap(true);

  this->_layout.addWidget(&this->_text);
  setLayout(&this->_layout);

  setGeometry(this->_screenWidth - this->_popupWidth,
              this->_screenHeight - this->_popupHeight,
              this->_popupWidth,
              this->_popupHeight);
  setLayout(&this->_layout);

  setMouseTracking(true);
  setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
}

void    Popup::buildCache(void)
{
  this->_cache = new QPixmap(size());
  this->_cache->fill(Qt::transparent);

  QPainter painter(this->_cache);

  // Window's background
  QPolygon background;

  background << QPoint(           0,            16)
             << QPoint(          16,             0)
             << QPoint(width() - 16,             0)
             << QPoint(width()     ,            16)
             << QPoint(width()     , height() - 16)
             << QPoint(width() - 16,      height())
             << QPoint(          16,      height())
             << QPoint(           0, height() - 16);

  painter.setPen  (QPen  (this->_backgroundColor));
  painter.setBrush(QBrush(this->_backgroundColor));

  painter.drawPolygon(background);

  // Window's frame
  QPolygon frame;

  frame << QPoint(           4,            20)
        << QPoint(          20,             4)
        << QPoint(width() - 20,             4)
        << QPoint(width() -  4,            20)
        << QPoint(width() -  4, height() - 20)
        << QPoint(width() - 20,  height() - 4)
        << QPoint(          20,  height() - 4)
        << QPoint(           4, height() - 20);

  painter.setPen(QPen(this->_frameColor));
  painter.setBrush(Qt::NoBrush);

  painter.drawPolygon(frame);
  update();
}
