#ifndef QSLIDING_POPUP_H_
#define QSLIDING_POPUP_H_

#include "QPopup.h"

class	QSlidingPopup : public QPopup
{
  Q_OBJECT

  public:
  QSlidingPopup(const int width, const int height);
  ~QSlidingPopup(void);

protected:
  virtual void	showAnimation(void);
  virtual void	stopAnimation(void);

protected slots:
  virtual void	hideAnimation(void);

private slots:
  virtual void	moveUp(void);
  virtual void	moveDown(void);

private:
  QTimer* _showAnimationTimer;
  QTimer* _hideAnimationTimer;
  QTimer* _stopShowingTimer;
  QTimer* _stopHidingTimer;
};

#endif
