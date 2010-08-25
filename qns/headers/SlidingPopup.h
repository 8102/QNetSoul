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

#ifndef SLIDING_POPUP_H_
#define SLIDING_POPUP_H_

#include "Popup.h"

class   SlidingPopup : public Popup
{
  Q_OBJECT

    public:
  SlidingPopup(const int width, const int height);
  ~SlidingPopup(void);

 protected:
  virtual void  showAnimation(void);
  virtual void  stopAnimation(void);

  protected slots:
  virtual void  hideAnimation(void);

  private slots:
  virtual void  moveUp(void);
  virtual void  moveDown(void);

 private:
  QTimer* _showAnimationTimer;
  QTimer* _hideAnimationTimer;
  QTimer* _stopShowingTimer;
  QTimer* _stopHidingTimer;
};

#endif
