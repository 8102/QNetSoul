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

#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QCryptographicHash>

#include "Url.h"
#include "Chat.h"
#include "Network.h"
#include "Pastebin.h"
#include "TrayIcon.h"
#include "QNetsoul.h"
#include "VieDeMerde.h"
#include "SlidingPopup.h"
#include "InternUpdater.h"
#include "OptionsWidget.h"
#include "ChuckNorrisFacts.h"
#include "PortraitResolver.h"
#include "Credentials.h"
#include "Singleton.hpp"
#include "tools.h"
#include "pluginsmanager.h"

// Imported from tools.cpp
extern const State states[];

QNetsoul::QNetsoul(void)
  : _network(new Network(this)), _options(new OptionsWidget(this)),
    _trayIcon(NULL), _portraitResolver(new PortraitResolver),
    _pastebin(new Pastebin), _popup(new SlidingPopup(300, 200)),
    _vdm(new VieDeMerde(this->_popup)),
    _cnf(new ChuckNorrisFacts(this->_popup)), _ping(new QTimer(this)),
    _internUpdater(new InternUpdater(this)),
    _pluginsManager(new PluginsManager)
{
  setupUi(this);
  setupTrayIcon();
  this->_pluginsManager->init(this->menuPlugins, this->_popup);
  this->_pluginsManager->loadDefaultDirectory();
  connectQNetsoulModules();
  connectActionsSignals();
  connectNetworkSignals();
  QWidget::setAttribute(Qt::WA_AlwaysShowToolTips);
  setWhatsThis(whatsThis()
               .replace("%CurrentVersion%", Tools::qnetsoulVersion())
               .replace("%Platform%", Tools::identifyPlatform(QNS_VERBOSE)));
  readSettings();
  this->tree->setOptions(this->_options);
  this->tree->setNetwork(this->_network);
  this->_network->setOptions(this->_options);
  this->tree->initTree();
  if (this->_options->mainWidget->autoConnect())
    connectToServer();
  this->_portraitResolver->addRequest(this->tree->getLoginList());
  const QString startWith = this->_options->funWidget->getStartingModule();
  if (startWith == QObject::tr("Vie de merde"))
    this->_vdm->getVdm();
  else if (startWith == QObject::tr("Chuck Norris facts"))
    this->_cnf->getFact();
  Credentials& instance = Singleton<Credentials>::Instance();
  instance.setOptions(this->_options);
  this->_internUpdater->checkLastVersion();
}

QNetsoul::~QNetsoul(void)
{
  delete this->_vdm;
  delete this->_cnf;
  delete this->_popup;
  delete this->_pastebin;
  delete this->_pluginsManager;
  delete this->_portraitResolver;
}

// Static
void    QNetsoul::openOptionsDialog(OptionsWidget* options,
                                    const int currentTab,
                                    QWidget* focus)
{
  Q_ASSERT(options);
  Q_ASSERT(currentTab < options->tabWidget->count());
  if (options->isVisible() == false)
    {
      options->updateOptions();
      if (currentTab != -1)
        options->tabWidget->setCurrentIndex(currentTab);
      if (focus)
        {
          focus->setFocus();
          if (currentTab == 0)
            options->mainWidget->setConnectionOnOk(true);
        }
      else
        {
          options->serverLineEdit->setFocus();
        }
      options->show();
    }
}

void    QNetsoul::closeEvent(QCloseEvent* event)
{
  static volatile bool firstTime = true;
  if (NULL == this->_trayIcon)
    {
      event->accept();
      return;
    }
  if (this->_trayIcon->isVisible())
    {
      this->_oldPos = this->pos();
      hide();
      if (true == firstTime)
        {
          firstTime = false;
          this->_trayIcon->showMessage("QNetSoul",
                                       tr("QNetSoul is still running."),
                                       5000);
        }
      event->ignore();
    }
}

void    QNetsoul::connectToServer(void)
{
  if (QAbstractSocket::ConnectedState == this->_network->state())
    return;

  if (!this->_options->loginLineEdit->text().isEmpty())
    {
      if (!this->_options->passwordLineEdit->text().isEmpty())
        {
          bool ok;
          quint16 port = this->_options->portLineEdit->text().toUShort(&ok);
          if (ok)
            {
              this->_network->connect(this->_options->serverLineEdit->text(),
                                      port);
              return;
            }
          else
            {
              QMessageBox::warning(this, "QNetSoul", tr("Port is invalid."));
              openOptionsDialog(this->_options, 0,
                                this->_options->portLineEdit);
            }
        }
      else
        {
          QMessageBox::warning(this, "QNetSoul",
                               tr("Your password is missing."));
          openOptionsDialog(this->_options, 0,
                            this->_options->passwordLineEdit);
        }
    }
  else
    {
      QMessageBox::warning(this, "QNetSoul", tr("Your login is missing."));
      openOptionsDialog(this->_options, 0,
                        this->_options->loginLineEdit);
    }
}

void    QNetsoul::ping(void)
{
  Q_ASSERT(this->_network);
#ifndef QT_NO_DEBUG
  qDebug() << "[QNetsoul::ping] Pingin'...";
#endif
  this->_network->sendMessage("ping\n");
  this->tree->refreshContacts();
}

void    QNetsoul::reconnect(void)
{
#ifndef QT_NO_DEBUG
  qDebug() << "[QNetsoul::reconnect] Reconnecting...";
#endif
  this->statusbar->showMessage(tr("Reconnecting..."));
  disconnect();
  connectToServer();
}

void    QNetsoul::disconnect(void)
{
  this->_ping->stop();
  resetAllContacts();
  this->_network->disconnect();
}

void    QNetsoul::updateWidgets(const QAbstractSocket::SocketState& state)
{
  if (QAbstractSocket::ConnectedState == state)
    {
      // MenuBar
      actionConnect->setEnabled(false);
      actionDisconnect->setEnabled(true);
      actionRefresh->setEnabled(true);
      // StatusBar state notifications are now handled in Network module
      // ComboBox
      this->statusComboBox->setEnabled(true);
      // TrayIconMenu
      if (this->_trayIcon)
        this->_trayIcon->setEnabledStatusMenu(true);
    }
  else if (QAbstractSocket::UnconnectedState == state)
    {
      // MenuBar
      actionConnect->setEnabled(true);
      actionDisconnect->setEnabled(false);
      actionRefresh->setEnabled(false);
      // StatusBar state notifications are now handled in Network module
      // ComboBox
      this->statusComboBox->setEnabled(false);
      this->statusComboBox->setCurrentIndex(0);
      // TrayIconMenu
      if (this->_trayIcon)
        this->_trayIcon->setEnabledStatusMenu(false);
    }
}

void    QNetsoul::openOptionsDialog(void)
{
  openOptionsDialog(this->_options);
}

// Disable all chats linked with this login removed from ContactsTree
void    QNetsoul::disableChats(const QString& login)
{
  QHashIterator<int, Chat*> i(this->_windowsChat);
  while (i.hasNext())
    {
      i.next();
      if (login == i.value()->login())
        disableChat(i.value());
    }
}

void    QNetsoul::saveStateBeforeQuiting(void)
{
  this->tree->saveContacts();
  writeSettings();
  qApp->quit();
}

void    QNetsoul::handleClicksOnTrayIcon
(QSystemTrayIcon::ActivationReason reason)
{
  if (QSystemTrayIcon::Trigger == reason)
    {
      if (this->isVisible())
        {
          this->_oldPos = this->pos();
          this->hide();
        }
      else
        {
          this->show();
        }
    }
}

// Connected with SIGNAL(state(const QStringList&))
// properties.at(0): Login
// properties.at(1): Id
// properties.at(2): Ip
// properties.at(3): Promo
// properties.at(4): State
// properties.at(5): Location
// properties.at(6): Comment
void    QNetsoul::changeStatus(const QStringList& properties)
{
  bool ok;
  Chat* chat = NULL;
  int id = properties.at(1).toInt(&ok);
  if (ok)
    chat = getChat(id);
  else
    qFatal("[QNetSoul::changeStatus] "
           "properties.at(1) must be a number. "
           "current value == %s",
           properties.at(1).toStdString().c_str());

  if (chat == NULL)
    chat = createWindowChat(id, properties.at(0), properties.at(5));

  for (int i = 0; (states[i].state); ++i)
    if (properties.at(4) == states[i].state)
      {
        chat->statusLabel->setPixmap(QPixmap(states[i].pixmap));
        if ("login" == properties.at(4))
          // get comment field
          this->_network->refreshContact(properties.at(0));
        else if ("logout" == properties.at(4))
          {
            disableChat(chat);
          }
        if (this->_trayIcon && this->_options->chatWidget->notifyState())
          this->_trayIcon->showMessage
            (this->tree->getAliasByLogin(properties.at(0)),
             tr("is now ") + states[i].displayState);
        break;
      }
  this->tree->updateConnectionPoint(properties);
}

// Connected with SIGNAL(who(const QStringList&))
// properties.at(0): Login
// properties.at(1): Id
// properties.at(2): Ip
// properties.at(3): Promo
// properties.at(4): State
// properties.at(5): Location
// properties.at(6): Comment
void    QNetsoul::updateContact(const QStringList& properties)
{
  bool ok;
  Chat* chat = NULL;
  int id = properties.at(1).toInt(&ok);
  if (ok)
    chat = getChat(id);
  else
    qFatal("[QNetSoul::updateContact]"
           "properties.at(1) a.k.a id must be a number."
           "current value == %s",
           properties.at(1).toStdString().c_str());

  if (chat == NULL)
    chat = createWindowChat(id, properties.at(0), properties.at(5));

  for (int i = 0; (states[i].state); ++i)
    if (properties.at(4) == states[i].state)
      chat->statusLabel->setPixmap(QPixmap(states[i].pixmap));
  this->tree->updateConnectionPoint(properties);
}

// properties.at(0): Login
// properties.at(1): Id
// properties.at(2): Ip
// properties.at(3): Promo
// properties.at(4): State
// properties.at(5): Location
// properties.at(6): Comment
void    QNetsoul::showConversation(const QStringList& properties,
                                   const QString& message)
{
  bool ok;
  const int id = properties.at(1).toInt(&ok);
  if (ok == false)
    qFatal("[QNetsoul::showConversation] Invalid id (%d)", id);

  Chat* window = getChat(id);
  const bool userEvent = message.isEmpty();

  if (NULL == window)
    {
      // DEBUG focus
      //qDebug() << "CASE 1";
      window = createWindowChat(id, properties.at(0), properties.at(5));
      window->show();
    }
  if (false == window->isVisible())
    {
      // DEBUG focus
      //qDebug() << "CASE 2";
      //window->outputTextBrowser->clear();
      //window->inputTextEdit->clear();
      if (userEvent)
        {
          window->show();
          window->activateWindow();
          QApplication::setActiveWindow(window);
          window->inputTextEdit->setFocus();
        }
      else window->showMinimized();
    }
  else
    {
      // DEBUG focus
      //qDebug() << "CASE 3";
      if (userEvent)
        {
          window->showNormal();
          window->activateWindow();
          QApplication::setActiveWindow(window);
          window->inputTextEdit->setFocus();
        }
    }
  if (message.isEmpty() == false)
    {
      if (window)
        {
          window->insertMessage(properties.at(0), message, QColor(204, 0, 0));
          window->autoReply(statusComboBox->currentIndex());
          QApplication::alert(window);
        }
      if (this->_trayIcon && this->_options->chatWidget->notifyMsg())
        this->_trayIcon->showMessage(properties.at(0), tr(" is talking to you."));
    }
}

void    QNetsoul::processHandShaking(int step, QStringList args)
{
  static QByteArray sum;

#ifndef QT_NO_DEBUG
  //qDebug() << "[QNetsoul::processHandShaking] Step:" << step;
#endif

  switch (step)
    {
    case 0:
      {
        const QString password(this->_options->passwordLineEdit->text());
        if (!password.isEmpty() && args.size() > 3)
          {
            sum.clear();
            this->_timeStamp = args.at(5);
            sum.append(QString("%1-%2/%3%4")
                       .arg(args.at(2)).arg(args.at(3))
                       .arg(args.at(4)).arg(password));
            sum = QCryptographicHash::hash(sum, QCryptographicHash::Md5);
            this->_network->sendMessage("auth_ag ext_user none none\n");
          }
        break;
      }
    case 1:
      {
        QByteArray message;
        const QString hex = sum.toHex();
        QString location(this->_options->locationLineEdit->text());
        QString comment(this->_options->commentLineEdit->text());

        if (location.isEmpty() || location.contains("%L"))
          this->_network->resolveLocation(location);
        if (comment.isEmpty())
          comment = Tools::defaultComment();
        message.append("ext_user_log ");
        message.append(this->_options->loginLineEdit->text() + ' ');
        message.append(hex);
        message.append(' ');
        message.append(url_encode(location.toStdString().c_str()));
        //message.append(QUrl::toPercentEncoding(location.toLatin1()));
        message.append(' ');
        message.append(url_encode(comment.toStdString().c_str()));
        //message.append(QUrl::toPercentEncoding(comment.toLatin1()));
        message.append('\n');
        this->_network->sendMessage(message);
        break;
      }
    case 2:
      {
        QByteArray state;
        QDateTime  dt = QDateTime::currentDateTime();

        state.append("state actif:");
        state.append(QString::number(static_cast<uint>(dt.toTime_t())));
        state.append("\n");
        this->_network->sendMessage(state);
        this->tree->monitorContacts();
        this->tree->refreshContacts();
        this->_ping->start(10000); // every 10 seconds, ping the server
        this->statusbar->showMessage(tr("You are now NetSouled."), 2000);
        break;
      }
    case -1:
      {
        disconnect();
        this->statusbar->showMessage(tr("Authentification failed."));
        break;
      }
    default:;
    }
}

void    QNetsoul::notifyTypingStatus(const int id, const bool typing)
{
  Chat* chat = getChat(id);
  if (chat)
    chat->notifyTypingStatus(typing);
}

void    QNetsoul::setPortrait(const QString& login)
{
  QString portraitPath;

  if (PortraitResolver::isAvailable(portraitPath, login) == false)
    return;

  QHashIterator<int, Chat*> i(this->_windowsChat);
  while (i.hasNext())
    {
      i.next();
      if (login == i.value()->login())
        {
          i.value()->portraitLabel->setPixmap(QPixmap(portraitPath));
          i.value()->setWindowIcon(QIcon(portraitPath));
        }
    }
  this->tree->setPortrait(login, portraitPath);
}

void    QNetsoul::aboutQNetSoul(void)
{
  QMessageBox::about(this, "QNetSoul", this->whatsThis());
}

Chat*   QNetsoul::getChat(const int id)
{
  QHash<int, Chat*>::iterator it;
  it = this->_windowsChat.find(id);
  if (this->_windowsChat.end() == it)
    return NULL;
  return it.value();
}

void    QNetsoul::disableChat(Chat* chat)
{
  Q_ASSERT(chat != NULL);
  chat->setEnabled(false);
  this->_windowsChat.remove(chat->id());
  if (chat->isVisible())
    chat->setAttribute(Qt::WA_DeleteOnClose);
  else delete chat;
}

void    QNetsoul::resetAllContacts(void)
{
  this->tree->removeAllConnectionPoints();
  QHash<int, Chat*>::iterator it = this->_windowsChat.begin();
  QHash<int, Chat*>::iterator end = this->_windowsChat.end();
  for (; it != end; ++it)
    it.value()->statusLabel->setPixmap(QPixmap(":/images/offline.png"));
}

void    QNetsoul::readSettings(void)
{
  QSettings settings("QNetSoul", "QNetsoul");

  settings.beginGroup("MainWindow");
  resize(settings.value("size", QSize(280, 584)).toSize());
  move(settings.value("pos", QPoint(501, 232)).toPoint());
  settings.endGroup();
}

void    QNetsoul::writeSettings(void)
{
  QSettings settings("QNetSoul", "QNetsoul");

  settings.beginGroup("MainWindow");
  settings.setValue("size", size());
  if (this->isVisible())
    settings.setValue("pos", pos());
  else
    settings.setValue("pos", this->_oldPos);
  settings.endGroup();
}

void    QNetsoul::setupTrayIcon(void)
{
  if (QSystemTrayIcon::isSystemTrayAvailable())
    {
      this->_trayIcon = new TrayIcon(this);
      this->_portraitResolver->setTrayIcon(this->_trayIcon);
      this->_internUpdater->setTrayIcon(this->_trayIcon);
    }
}

void    QNetsoul::connectQNetsoulModules(void)
{
  connect(this->_ping, SIGNAL(timeout()), this, SLOT(ping()));
  connect(this->_internUpdater, SIGNAL(quitApplication()),
          this, SLOT(saveStateBeforeQuiting()));
  connect(this->_portraitResolver,
          SIGNAL(downloadedPortrait(const QString&)),
          SLOT(setPortrait(const QString&)));
  connect(this->tree, SIGNAL(openConversation(const QStringList&)),
          this, SLOT(showConversation(const QStringList&)));
  connect(this->tree, SIGNAL(downloadPortrait(const QString&, bool)),
          this->_portraitResolver, SLOT(addRequest(const QString&, bool)));
  connect(this->tree, SIGNAL(contactRemoved(const QString&)),
          SLOT(disableChats(const QString&)));
}

void    QNetsoul::connectActionsSignals(void)
{
  // QNetsoul
  connect(actionConnect, SIGNAL(triggered()), SLOT(connectToServer()));
  connect(actionDisconnect, SIGNAL(triggered()), SLOT(disconnect()));
  connect(actionCheckForUpdates, SIGNAL(triggered()),
          this->_internUpdater, SLOT(startUpdater()));
  connect(actionQuit, SIGNAL(triggered()), SLOT(saveStateBeforeQuiting()));
  // Contacts
  connect(actionAddG, SIGNAL(triggered()), this->tree, SLOT(addGroup()));
  connect(actionAddC, SIGNAL(triggered()), this->tree, SLOT(addContact()));
  connect(actionRefresh, SIGNAL(triggered()),
          this->tree, SLOT(refreshContacts()));
  connect(actionLoadContacts, SIGNAL(triggered()),
          this->tree, SLOT(loadContacts()));
  connect(actionSaveContacts, SIGNAL(triggered()),
          this->tree, SLOT(saveContacts()));
  connect(actionSaveContactsAs, SIGNAL(triggered()),
          this->tree, SLOT(saveContactsAs()));
  // Plugins
  connect(actionPluginsManager, SIGNAL(triggered()),
          this->_pluginsManager, SLOT(show()));
  connect(actionVDM, SIGNAL(triggered()), this->_vdm, SLOT(getVdm()));
  connect(actionCNF, SIGNAL(triggered()), this->_cnf, SLOT(getFact()));
  connect(actionPastebin, SIGNAL(triggered()),
          this->_pastebin, SLOT(pastebinIt()));
  // Options
  connect(actionPreferences, SIGNAL(triggered()), SLOT(openOptionsDialog()));
  // Help
  connect(actionAbout_QNetSoul, SIGNAL(triggered()), SLOT(aboutQNetSoul()));
  connect(actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  // Status
  connect(statusComboBox, SIGNAL(currentIndexChanged(int)),
          this->_network, SLOT(sendStatus(const int&)));
  // From Option widget
  connect(this->_options->mainWidget,
          SIGNAL(loginPasswordFilled()), SLOT(connectToServer()));
}

void    QNetsoul::connectNetworkSignals(void)
{
  connect(this->_network, SIGNAL(handShaking(int, QStringList)),
          SLOT(processHandShaking(int, QStringList)));
  connect(this->_network, SIGNAL(msg(const QStringList&, const QString&)),
          SLOT(showConversation(const QStringList&, const QString&)));
  connect(this->_network, SIGNAL(state(const QStringList&)),
          SLOT(changeStatus(const QStringList&)));
  connect(this->_network, SIGNAL(who(const QStringList&)),
          SLOT(updateContact(const QStringList&)));
  connect(this->_network, SIGNAL(typingStatus(const int, bool)),
          SLOT(notifyTypingStatus(const int, bool)));
}

Chat*   QNetsoul::createWindowChat(const int id,
                                   const QString& login,
                                   const QString& location)
{
  Chat* chat = new Chat(id, login, location);
  chat->setOptions(this->_options);
  chat->setNetwork(this->_network);
  // Binding shortcuts
  chat->addAction(this->actionVDM);
  chat->addAction(this->actionCNF);
  chat->addAction(this->actionPastebin);
  chat->addAction(this->actionPreferences);
  this->_windowsChat.insert(id, chat);
  return chat;
}

void    QNetsoul::deleteAllWindowChats(void)
{
  QHash<int, Chat*>::const_iterator cit = this->_windowsChat.constBegin();
  for (; cit != this->_windowsChat.constEnd(); ++cit)
    delete cit.value();
}
