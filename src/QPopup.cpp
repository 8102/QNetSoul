#include <iostream>
#include <QPainter>
#include "QPopup.h"

namespace
{
  qreal INITIAL_OPACITY = 0.9;
  qreal HOVER_OPACITY = 0.2;
}

QPopup::QPopup(const int width, const int height)
  : _popupWidth(width), _popupHeight(height),
    _textColor("#CC6633"), _backgroundColor("#171722"),
    _frameColor("#B1B1CB"), _cache(NULL)
{
  init();
  this->_hiddingTrigger = new QTimer(this);
  this->_hiddingTrigger->setSingleShot(true);
  connect(this->_hiddingTrigger, SIGNAL(timeout()), SLOT(hideAnimation()));
}

QPopup::~QPopup(void)
{
}

void	QPopup::showUp(const QString& text, const int timeout)
{
  if (isVisible()) stopAnimation();

  setWindowOpacity(INITIAL_OPACITY);
  this->_text.setText("<font color=" + this->_textColor.name() + '>' +
		      text + "</font>");
  this->_hiddingTrigger->setInterval(timeout);
  this->_hiddingTrigger->start();
  showAnimation();
}

void	QPopup::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);

  if (this->_cache != NULL)
    {
      QPainter painter(this);
      painter.drawPixmap(0, 0, *this->_cache);
    }
  else
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
		 << QPoint(           0, height() -  16);

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
	    << QPoint(           4, height() -  20);

      painter.setPen(QPen(this->_frameColor));
      painter.setBrush(Qt::NoBrush);

      painter.drawPolygon(frame);
      update();
    }
}

void	QPopup::mousePressEvent(QMouseEvent* event)
{
  Q_UNUSED(event);
  stopAnimation();
}

void	QPopup::mouseMoveEvent(QMouseEvent* event)
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

void	QPopup::showAnimation(void)
{
  show();
}

void	QPopup::stopAnimation(void)
{
  this->_hiddingTrigger->stop();
  hide();
}

void	QPopup::hideAnimation(void)
{
  hide();
}

void	QPopup::init(void)
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

  setWindowFlags(Qt::Popup);
//   setWindowFlags(Qt::Popup |
// 		 Qt::CustomizeWindowHint |
// 		 Qt::FramelessWindowHint);

  setMouseTracking(true);
  setAttribute(Qt::WA_TranslucentBackground);
}
