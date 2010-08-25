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

#ifndef POPUP_H_
#define POPUP_H_

#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QTextEdit>
#include <QMouseEvent>
#include <QGridLayout>
#include <QApplication>
#include <QDesktopWidget>

class   Popup : public QWidget
{
  Q_OBJECT

    public:
  Popup(const int width, const int height);
  ~Popup(void);

  void  setTextColor(const QColor& c) { this->_textColor = c; }
  void  setBackgroundColor(const QColor& c) { this->_backgroundColor = c; }
  void  setFrameColor(const QColor& c) { this->_frameColor = c; }

  void  showUp(const QString& text, const int timeout = 10000);

 protected:
  virtual void  paintEvent(QPaintEvent* event);
  virtual void  mousePressEvent(QMouseEvent* event);
  virtual void  mouseMoveEvent(QMouseEvent* event);
  virtual void  showAnimation(void);
  virtual void  stopAnimation(void);

  protected slots:
  virtual void  hideAnimation(void);

 private:
  void  init(void);
  void  buildCache(void);

 protected:
  int _popupWidth;
  int _popupHeight;
  int _screenWidth;
  int _screenHeight;

  QLabel _text;
  QColor _textColor;
  QColor _backgroundColor;
  QColor _frameColor;
  QGridLayout _layout;
  QPixmap* _cache;
  QTimer* _hiddingTrigger;
};

#endif
