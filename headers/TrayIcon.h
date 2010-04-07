#ifndef TRAY_ICON_H_
#define TRAY_ICON_H_

#include <QMenu>
#include <QTimer>
#include <QSystemTrayIcon>
#include "QNetsoul.h"

class	QNetsoul;

class	TrayIcon : public QSystemTrayIcon
{
  Q_OBJECT

  public:
  TrayIcon(QNetsoul* parent);
  ~TrayIcon(void);

  void	showMessage(const QString& t, const QString& m, int timeout = 10000);

  void	setEnabledStatusMenu(bool enabled);
  void	startBlinking(void);
  void	stopBlinking(void);

private slots:
  void	blink(void);
  void	changeStatus(void);

private:
  void	setupTrayIcon(QNetsoul* parent);

private:
  QMenu*	_statusMenu;
  QTimer	_blinkingTimer;

signals:
  void	moveComboBox(int);
};

#endif
