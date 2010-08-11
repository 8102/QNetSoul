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

#include <iostream>
#include <QByteArray>
#include "Url.h"
#include "QNetsoul.h"

namespace
{
  struct State
  {
    const char*   state;
    const char*   pixmap;
    const QString displayState;
  };
  const State   states[] =
    {
      {"login",         ":/images/log-in",      QObject::tr("Login")},
      {"logout",        ":/images/offline",     QObject::tr("Offline")},
      {"actif",         ":/images/online",      QObject::tr("Online")},
      {"away",          ":/images/away",        QObject::tr("Away")},
      {"idle",          ":/images/away",        QObject::tr("Idle")},
      {"lock",          ":/images/lock",        QObject::tr("Locked")},
      {"server",        ":/images/server",      QObject::tr("Server")},
      {NULL, NULL, NULL}
    };
}

QNetsoul::QNetsoul(QWidget* parent)
  :     QMainWindow(parent),
        _network(new Network(this)),
        _options(new Options(this)),
        _trayIcon(NULL),
        _popup(300, 200),
        _vdm(&this->_popup),
        _cnf(&this->_popup)
{
  setupUi(this);
  if (QSystemTrayIcon::isSystemTrayAvailable())
    this->_trayIcon = new TrayIcon(this);
  connectQNetsoulItems();
  connectActionsSignals();
  connectNetworkSignals();
  QObject::connect(&this->_ping, SIGNAL(timeout()), this, SLOT(ping()));
  QWidget::setAttribute(Qt::WA_AlwaysShowToolTips);
  setWhatsThis(whatsThis().replace("%CurrentVersion%", currentVersion()));
  readSettings();
  this->tree->setOptions(this->_options);
  this->_network->setOptions(this->_options);
  if (QDir(QDir::currentPath()).exists("contacts.qns"))
    this->tree->loadContacts("contacts.qns");
  this->_portraitResolver.addRequest(this->tree->getLoginList());
  if (this->_options->mainWidget->autoConnect())
    connectToServer();
  const QString startWith = this->_options->funWidget->getStartingModule();
  if (startWith == QObject::tr("Vie de merde"))
    this->_vdm.getVdm();
  else if (startWith == QObject::tr("Chuck Norris facts"))
    this->_cnf.getFact();
}

QNetsoul::~QNetsoul(void)
{
  delete this->_trayIcon;
}

void    QNetsoul::closeEvent(QCloseEvent* event)
{
  static volatile bool  firstTime = true;

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
              this->statusbar->showMessage(tr("Connecting..."), 3000);
              this->_network->connect(this->_options->serverLineEdit->text(),
                                      port);
              return;
            }
          else
            {
              QMessageBox::warning(this, "QNetSoul", tr("Port is invalid."));
              openOptionsDialog(this->_options->portLineEdit);
            }
        }
      else
        {
          QMessageBox::warning(this, "QNetSoul",
                               tr("Your password is missing."));
          openOptionsDialog(this->_options->passwordLineEdit);
        }
    }
  else
    {
      QMessageBox::warning(this, "QNetSoul", tr("Your login is missing."));
      openOptionsDialog(this->_options->loginLineEdit);
    }
}

void    QNetsoul::ping(void)
{
  Q_ASSERT(this->_network);
  this->_network->sendMessage("ping\n");
}

void    QNetsoul::reconnect(void)
{
#ifndef QT_NO_DEBUG
  qDebug() << "[QNetsoul::reconnect] Reconnecting...";
#endif
  disconnect();
  connectToServer();
}

void    QNetsoul::disconnect(void)
{
  this->_ping.stop();
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
      // StatusBar
      this->statusbar->showMessage(tr("Connected"));
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
      // StatusBar
      this->statusbar->showMessage(tr("Disconnected"));
      // ComboBox
      this->statusComboBox->setEnabled(false);
      this->statusComboBox->setCurrentIndex(0);
      // TrayIconMenu
      if (this->_trayIcon)
        this->_trayIcon->setEnabledStatusMenu(false);
    }
}

void    QNetsoul::checkForUpdates(void)
{
#ifndef QT_NO_DEBUG
  qDebug() << "[QNetsoul::checkForUpdates]"
           << "Fill here :)";
#endif
}

void    QNetsoul::saveStateBeforeQuiting(void)
{
  if (this->tree->topLevelItemCount() > 0)
    this->tree->saveContacts("contacts.qns");
  writeSettings();
  qApp->quit();
}

void    QNetsoul::openOptionsDialog(QLineEdit* newLineFocus)
{
  if (this->_options->isVisible() == false)
    {
      this->_options->updateOptions();
      this->_options->tabWidget->setCurrentIndex(0);
      if (newLineFocus != NULL)
        {
          newLineFocus->setFocus();
          this->_options->mainWidget->setConnectionOnOk(true);
        }
      else
        {
          this->_options->serverLineEdit->setFocus();
        }
      this->_options->show();
    }
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

void    QNetsoul::sendStatus(const int& status) const
{
  switch (status)
    {
    case 0: this->_network->sendMessage("state actif\n"); break;
    case 1: this->_network->sendMessage("state lock\n"); break;
    case 2: this->_network->sendMessage("state away\n"); break;
    case 3: this->_network->sendMessage("state server\n"); break;
    default: qFatal("[QNetSoul::sendStatus] unknown state: %d", status);
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
          refreshContact(properties.at(0)); // get comment field
        else if ("logout" == properties.at(4))
          {
            chat->setEnabled(false);
            this->_windowsChat.remove(id);
            if (chat->isVisible())
              chat->setAttribute(Qt::WA_DeleteOnClose);
            else
              delete chat;
          }
        if (this->_trayIcon && this->_options->chatWidget->notifyState())
          this->_trayIcon->showMessage(properties.at(0),
                                       tr("is now ") +
                                       states[i].displayState);
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
    qFatal("[QNetSoul::updateContact] "
           "properties.at(1) must be a number. "
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
      // DEBUG
      //qDebug() << "CASE 1";
      window = createWindowChat(id, properties.at(0), properties.at(5));
      if (userEvent)
        {
          window->setVisible(true);
          QApplication::setActiveWindow(window);
        }
      else
        {
          window->showMinimized();
        }
    }
  else
    {
      if (false == window->isVisible())
        {
          // DEBUG
          //qDebug() << "CASE 2";
          window->outputTextBrowser->clear();
          window->inputTextEdit->clear();
          window->inputTextEdit->setFocus();
          if (userEvent)
            {
              window->show();
              window->activateWindow();
              window->raise();
            }
          else
            window->showMinimized();
        }
      else
        {
          // DEBUG
          //qDebug() << "CASE 3";
          if (userEvent)
            {
              window->showNormal();
              //window->hide();
              window->show();
              window->activateWindow();
              window->raise();
              //QApplication::setActiveWindow(window);
            }
        }
    }
  if (message.isEmpty() == false)
    {
      const int index = statusComboBox->currentIndex();

      if (window)
        window->insertMessage(properties.at(0), message, QColor(204, 0, 0));
      if (index > 0)
        {
          const QString autoReply =
            this->_options->chatWidget->getReply(index - 1);
          if (autoReply.isEmpty() == false)
            transmitMsg(id, autoReply);
        }
      else if (this->_trayIcon)
        {
          if (this->_options->chatWidget->notifyMsg())
            this->_trayIcon->showMessage(properties.at(0),
                                         tr(" is talking to you."));
        }
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
        const QString   password = this->_options->passwordLineEdit->text();
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
          comment = QNetsoul::defaultComment();
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
        QByteArray      state;
        QDateTime       dt = QDateTime::currentDateTime();

        state.append("state actif:");
        state.append(QString::number(static_cast<uint>(dt.toTime_t())));
        state.append("\n");
        this->_network->sendMessage(state);
        watchLogContacts();
        refreshContacts();
        this->_ping.start(10000); // every 10 seconds, ping the server
        this->statusbar->showMessage(tr("You are now Netsouled."), 2000);
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

void    QNetsoul::transmitMsg(const int id, const QString& msg)
{
  Chat* chat = getChat(id);

  if (chat)
    {
      chat->insertMessage(this->_options->loginLineEdit->text(),
                          msg, QColor(32, 74, 135));
    }
  QByteArray result;
  result.append("user_cmd msg ");
  result.append("*:" + chat->login());
  result.append("@*");
  result.append(url_encode(chat->location().toStdString().c_str()));
  result.append("*");
  result.append(" msg ");
  result.append(url_encode(msg.toStdString().c_str()));
  result.append('\n');
  this->_network->sendMessage(result);
}

// TODO: move this function to Network module
void    QNetsoul::transmitTypingStatus(const QString& login, bool status)
{
  QByteArray cmd("user_cmd msg_user ");
  cmd += login;
  if (!status)
    cmd += " dotnetSoul_UserTyping null\n";
  else
    cmd += " dotnetSoul_UserCancelledTyping null\n";
  this->_network->sendMessage(cmd.data());
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

void    QNetsoul::watchLogContacts(void)
{
  const QStringList list = this->tree->getLoginList();
  const int size = list.size();

  if (size > 0)
    {
      QByteArray netMsg("user_cmd watch_log_user {");
      for (int i = 0; i < size; ++i)
        {
          netMsg.append(list[i]);
          if (i + 1 < size)
            netMsg.append(',');
        }
      netMsg.append("}\n");
      this->_network->sendMessage(netMsg);
    }
}

void    QNetsoul::watchLogContact(const QString& contact)
{
  QByteArray    netMsg("user_cmd watch_log_user ");
  netMsg.append(contact + '\n');
  this->_network->sendMessage(netMsg);
}

// TODO: move this function to Network module
void    QNetsoul::refreshContact(const QString& contact)
{
  QByteArray    message("user_cmd who ");
  message.append(contact + '\n');
  this->_network->sendMessage(message);
  this->_portraitResolver.addRequest(contact, false);
}

void    QNetsoul::refreshContacts(void)
{
  const QStringList contacts = this->tree->getLoginList();
  const int size = contacts.size();

  if (size == 0) return;

  QByteArray netMsg("user_cmd who {");
  for (int i = 0; i < size; ++i)
    {
      netMsg.append(contacts.at(i));
      if (i + 1 < size)
        netMsg.append(',');
    }
  netMsg.append("}\n");
  this->_network->sendMessage(netMsg);
}

void    QNetsoul::resetAllContacts(void)
{
  this->tree->removeAllConnectionPoints();
  QHash<int, Chat*>::iterator it = this->_windowsChat.begin();
  QHash<int, Chat*>::iterator end = this->_windowsChat.end();
  for (; it != end; ++it)
    it.value()->statusLabel->setPixmap(QPixmap(":/images/offline"));
}

void    QNetsoul::readSettings(void)
{
  QSettings settings("Epitech", "QNetsoul");

  settings.beginGroup("MainWindow");
  resize(settings.value("size", QSize(240, 545)).toSize());
  move(settings.value("pos", QPoint(501, 232)).toPoint());
  settings.endGroup();
}

void    QNetsoul::writeSettings(void)
{
  QSettings settings("Epitech", "QNetsoul");

  settings.beginGroup("MainWindow");
  settings.setValue("size", size());
  if (this->isVisible())
    settings.setValue("pos", pos());
  else
    settings.setValue("pos", this->_oldPos);
  settings.endGroup();
}

void    QNetsoul::connectQNetsoulItems(void)
{
  connect(&this->_portraitResolver,
          SIGNAL(downloadedPortrait(const QString&)),
          SLOT(setPortrait(const QString&)));
  connect(this->tree, SIGNAL(openConversation(const QStringList&)),
          this, SLOT(showConversation(const QStringList&)));
  connect(this->tree, SIGNAL(downloadPortrait(const QString&)),
          &this->_portraitResolver, SLOT(addRequest(const QString&)));
}

void    QNetsoul::connectActionsSignals(void)
{
  // QNetsoul
  connect(actionConnect, SIGNAL(triggered()), SLOT(connectToServer()));
  connect(actionDisconnect, SIGNAL(triggered()), SLOT(disconnect()));
  connect(actionCheckForUpdates, SIGNAL(triggered()),
          SLOT(checkForUpdates()));
  connect(actionQuit, SIGNAL(triggered()), SLOT(saveStateBeforeQuiting()));
  // Contacts
  connect(actionAddG, SIGNAL(triggered()), this->tree, SLOT(addGroup()));
  connect(actionAddC, SIGNAL(triggered()), this->tree, SLOT(addContact()));
  connect(actionRefresh, SIGNAL(triggered()), SLOT(refreshContacts()));
  connect(actionLoadContacts, SIGNAL(triggered()),
          this->tree, SLOT(loadContacts()));
  connect(actionSaveContacts, SIGNAL(triggered()),
          this->tree, SLOT(saveContacts()));
  // Featurettes
  connect(actionVDM, SIGNAL(triggered()), &this->_vdm, SLOT(getVdm()));
  connect(actionCNF, SIGNAL(triggered()), &this->_cnf, SLOT(getFact()));
  connect(actionPastebin, SIGNAL(triggered()),
          &this->_pastebin, SLOT(pastebinIt()));
  // Options
  connect(actionPreferences, SIGNAL(triggered()), SLOT(openOptionsDialog()));
  // Help
  connect(actionAbout_QNetSoul, SIGNAL(triggered()), SLOT(aboutQNetSoul()));
  connect(actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  // Status
  connect(statusComboBox, SIGNAL(currentIndexChanged(int)),
          SLOT(sendStatus(const int&)));
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
  chat->inputTextEdit->setFocus();
  this->_windowsChat.insert(id, chat);
  connect(chat, SIGNAL(msgToSend(const int, const QString&)),
          this, SLOT(transmitMsg(const int, const QString&)));
  connect(chat, SIGNAL(typingSignal(const QString&, bool)),
          this, SLOT(transmitTypingStatus(const QString&, bool)));
  return chat;
}

void    QNetsoul::deleteAllWindowChats(void)
{
  QHash<int, Chat*>::const_iterator cit =
    this->_windowsChat.constBegin();

  for (; cit != this->_windowsChat.constEnd(); ++cit)
    delete cit.value();
}
