#ifndef QPOPUP_H_
#define QPOPUP_H_

#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QTextEdit>
#include <QMouseEvent>
#include <QGridLayout>
#include <QApplication>
#include <QDesktopWidget>

class   QPopup : public QWidget
{
  Q_OBJECT

    public:
  QPopup(const int width, const int height);
  ~QPopup(void);

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
