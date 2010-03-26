/*
  Copyright 2009 Dally Richard
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
#include "Status.h"
#include "QNetsoul.h"
#include "ContactWidget.h"
#include "ContactsReader.h"
#include "ContactsWriter.h"
#include "ConnectionPoint.h"

const char*	ONLINE = "Online";
const char*	OFFLINE = "Offline";

QNetsoul::QNetsoul(QWidget* parent)
  :	QMainWindow(parent),
	_network(new Network(this)),
	_options(new Options(this)),
	_addContact(new AddContact(this)),
	_standardItemModel(new QStandardItemModel(this)),
	_trayIcon(NULL),
	_typingNotification(false)
{
  setupUi(this);
  setupTrayIcon();
  setupStatusButton();
  this->contactsTreeView->setModel(this->_standardItemModel);
  connectQNetsoulItems();
  connectActionsSignals();
  connectNetworkSignals();
  readSettings();
  loadContacts("contacts.xml");
  this->_options->applyOptions(); // Version 0.04
  //configureProxy(); // perhaps useless in the future...
  this->_portraitResolver.addRequest(getContactLogins());
  this->_vdm.getALife();
  //this->_cnf.getFact();
  //connect(this->_timer, SIGNAL(timeout()), SLOT(refreshContacts()));
  //this->contactsTreeView->setAttribute(Qt::WA_ShowWithoutActivating, true);
  //this->contactsTreeView->setAttribute(Qt::WA_AlwaysShowToolTips, true);
}

QNetsoul::~QNetsoul(void)
{
}

void	QNetsoul::closeEvent(QCloseEvent* event)
{
  static volatile bool	firstTime = true;

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
	  this->_trayIcon->showMessage("QNetSoul", tr("QNetSoul is still running."),
				       QSystemTrayIcon::Information, 5000);
        }
      event->ignore();
    }
}

void	QNetsoul::connectToServer(void)
{
  if (tr(ONLINE) == this->_statusPushButton->text())
    return;

  if (!this->_options->loginLineEdit->text().isEmpty())
    {
      if (!this->_options->passwordLineEdit->text().isEmpty())
        {
	  bool		ok;
	  quint16	port = this->_options->portLineEdit->text().toUShort(&ok);
	  if (ok)
            {
	      this->statusbar->showMessage(tr("Connecting..."), 3000);
	      this->_network->connect(this->_options->serverLineEdit->text(), port);
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
	  QMessageBox::warning(this, "QNetSoul", tr("Your password is missing."));
	  openOptionsDialog(this->_options->passwordLineEdit);
        }
    }
  else
    {
      QMessageBox::warning(this, "QNetSoul", tr("Your login is missing."));
      openOptionsDialog(this->_options->loginLineEdit);
    }
}

void	QNetsoul::reconnect(void)
{
  disconnect();
  connectToServer();
}

void	QNetsoul::disconnect(void)
{
  resetAllContacts();
  this->_network->disconnect();

}

void	QNetsoul::updateMenuBar(const QAbstractSocket::SocketState& state)
{
  if (QAbstractSocket::ConnectedState == state)
    {
      actionConnect->setEnabled(false);
      actionDisconnect->setEnabled(true);
      actionRefresh->setEnabled(true);
    }
  else if (QAbstractSocket::UnconnectedState == state)
    {
      actionConnect->setEnabled(true);
      actionDisconnect->setEnabled(false);
      actionRefresh->setEnabled(false);
    }
}

void	QNetsoul::updateStatusBar(const QAbstractSocket::SocketState& state)
{
  if (QAbstractSocket::ConnectedState == state)
    {
      this->_statusPushButton->setText(tr(ONLINE));
      this->_statusPushButton->setToolTip(tr("Disconnect"));
    }
  else if (QAbstractSocket::UnconnectedState == state)
    {
      this->_statusPushButton->setText(tr(OFFLINE));
      this->_statusPushButton->setToolTip(tr("Get NetSouled !"));
      this->statusbar->showMessage(tr("Disconnected"), 2000);
    }
}

void	QNetsoul::updateStatusComboBox(const QAbstractSocket::SocketState& state)
{
  if (QAbstractSocket::ConnectedState == state)
    {
      this->statusComboBox->setEnabled(true);
    }
  else if (QAbstractSocket::UnconnectedState == state)
    {
      this->statusComboBox->setEnabled(false);
      this->statusComboBox->setCurrentIndex(0);
    }
}

void	QNetsoul::showVdmInBalloon(const QString& message)
{
  if (NULL != this->_trayIcon)
    {
      // DEBUG EXCEPTION v0.07
      try
	{
	  this->_trayIcon->showMessage("Vie de merde", message,
				       QSystemTrayIcon::NoIcon, 15000);
	}
      catch(...)
	{
	  std::cerr << "Crash detected in showVdmInBalloon..." << std::endl;
	}
    }
}

void	QNetsoul::toggleConnection(void)
{
  if (tr(OFFLINE) == this->_statusPushButton->text())
    {
      connectToServer();
    }
  else
    {
      disconnect();
    }
}

void	QNetsoul::saveStateBeforeQuiting(void)
{
  saveContacts("contacts.xml");
  writeSettings();
  qApp->quit();
}

void	QNetsoul::openAddContactDialog(void)
{
  if (this->_addContact->isVisible() == false)
    {
      this->_addContact->loginLineEdit->clear();
      this->_addContact->loginLineEdit->setFocus();
      this->_addContact->show();
    }
  else
    {
      this->_addContact->activateWindow();
    }
}

void	QNetsoul::openOptionsDialog(QLineEdit* newLineFocus)
{
  if (this->_options->isVisible() == false)
    {
      this->_options->update();
      if (NULL != newLineFocus)
        {
	  newLineFocus->setFocus();
	  this->_options->setConnectionOnOk(true);
        }
      else
        {
	  this->_options->serverLineEdit->setFocus();
        }
      this->_options->show();
    }
}

void	QNetsoul::loadContacts(void)
{
  const QString fileName =
    QFileDialog::getOpenFileName(this, tr("Open Contacts File"),
				 QDir::currentPath(),
				 tr("XML Files (*.xml)"));
  if (false == fileName.isEmpty())
    loadContacts(fileName);
}

void	QNetsoul::saveContactsAs(void)
{
  QString fileName =
    QFileDialog::getSaveFileName(this, tr("Save Contacts File"),
				 QDir::currentPath(),
				 tr("XML Files (*.xml)"));
  if (!fileName.isEmpty())
    saveContacts(fileName);
}

void	QNetsoul::toggleSortContacts(void)
{
  this->contactsTreeView->model()->sort(0, Qt::AscendingOrder);
}

void	QNetsoul::handleClicksOnTrayIcon(QSystemTrayIcon::ActivationReason reason)
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

void	QNetsoul::addContact(void)
{
  addContact(this->_addContact->loginLineEdit->text());
}

void	QNetsoul::addContact(const QList<Contact> list)
{
  const int	size = list.size();

  for (int i = 0; i < size; ++i)
    {
      addContact(list[i].login, list[i].alias);
    }
}

void	QNetsoul::addContact(const QString& login, const QString& alias)
{
  if (!login.isEmpty() && false == doesThisContactAlreadyExist(login))
    {
      this->_portraitResolver.addRequest(login, false);
      ContactWidget*	widget = new ContactWidget(this, login, alias);
      QStandardItem*	newItem = new QStandardItem;

      connect(widget, SIGNAL(doubleClick(const QString&, const QString&)),
	      this, SLOT(showConversation(const QString&, const QString&)));

      newItem->setSizeHint(widget->size());
      //this->_standardItemModel->insertRow(this->_standardItemModel->rowCount(), newItem);
      this->_standardItemModel->appendRow(newItem);
      this->contactsTreeView->setIndexWidget(newItem->index(), widget);
      this->_addContact->loginLineEdit->clear();
      if (QAbstractSocket::ConnectedState == this->_network->state())
        {
	  refreshContact(login);
	  watchLogContact(login);
        }
    }
}

void	QNetsoul::removeSelectedContact(void)
{
  const	int row = this->contactsTreeView->currentIndex().row();
  this->contactsTreeView->model()->removeRow(row);
}

void	QNetsoul::sendStatus(const int& status) const
{
  switch (status)
    {
    case 0: this->_network->sendMessage("state actif\n"); break;
    case 1: this->_network->sendMessage("state lock\n"); break;
    case 2: this->_network->sendMessage("state away\n"); break;
    case 3: this->_network->sendMessage("state server\n"); break;
    default: std::cerr << "Unknown state FIXME" << std::endl;
    }
}

// A REVOIR si ca ne marche pas...
void	QNetsoul::changeStatus(const QString& login, const QString& id, const QString& state)
{
  static const Status		status[] =
    {
      {"login",		QObject::tr("logged in")},
      {"logout",	QObject::tr("offline")},
      {"actif",		QObject::tr("online")},
      {"away",		QObject::tr("away")},
      {"lock",		QObject::tr("locked")},
      {"server",	QObject::tr("deamonized")},
      {NULL, NULL}
    };
  ContactWidget*	contactWidget = getContact(login);

  if (NULL != contactWidget)
    {
      if (NULL != this->_trayIcon)
        {
	  for (int i = 0; (status[i].state); ++i)
            {
	      if (state == status[i].state)
                {
		  this->_trayIcon->showMessage(login, tr("is now ") + status[i].translation, QSystemTrayIcon::NoIcon);
		  break;
                }
            }
        }
      if (0 == contactWidget->getConnectionsSize())
        {
	  refreshContact(login);
        }
      else
        {
	  contactWidget->updateConnectionPoint(id, state);

	  Chat*	chat = getChat(login);
	  if (chat)
            {
	      chat->statusLabel->setPixmap(contactWidget->getStatus());
            }
        }
    }
}

// id(4), login(5), ip(6), group(13), state(14), location(12), comment(15)
void	QNetsoul::updateContact(const QStringList& parts)
{
  ContactWidget*	contactWidget = getContact(parts.at(5));

  std::cerr << "Update contact " << parts.at(5).toStdString() << std::endl;
  if (contactWidget)
    {
      ConnectionPoint	connection = {parts.at(4), parts.at(6),
                                      parts.at(14).section(':', 0, 0),
                                      url_decode(parts.at(15).toStdString().c_str()),
                                      url_decode(parts.at(12).toStdString().c_str())};

      contactWidget->addConnectionPoint(connection);
      changeStatus(parts.at(5), parts.at(4), connection.state); // PATCH
      if (false == contactWidget->hasGroup())
	contactWidget->setGroup(parts.at(13));
    }
}

void	QNetsoul::showConversation(const QString& login, const QString& message)
{
  const bool	userEvent = message.isEmpty();
  Chat*		window = getChat(login);

  if (NULL == window)
    {
      ContactWidget*	contactWidget = getContact(login);

      // DEBUG
      //std::cerr << "CASE 1" << std::endl;
      window = createWindowChat(login);
      window->inputTextEdit->setFocus();
      if (contactWidget)
	window->statusLabel->setPixmap(contactWidget->getStatus());

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
	  //std::cerr << "CASE 2" << std::endl;
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
	  //std::cerr << "CASE 3" << std::endl;
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
      const int	index = statusComboBox->currentIndex();

      if (window)
	window->insertMessage(login, QString(url_decode(message.toStdString().c_str())), QColor(204, 0, 0));
      if (index > 0)
	{
	  const QString	autoReply = this->_options->getReply(index - 1);
	  if (autoReply.isEmpty() == false)
	    transmitMsg(login, autoReply);
	}
      else if (this->_trayIcon)
	this->_trayIcon->showMessage(login, tr(" is talking to you."));
    }
}

void	QNetsoul::processHandShaking(int step, QStringList args)
{
  static QByteArray	sum;
  // DEBUG
  //std::cerr << "Step: " << step << std::endl;

  switch (step)
    {
    case 0:
      {
	const QString	password = this->_options->passwordLineEdit->text();
	if (!password.isEmpty() && args.size() > 3)
	  {
	    sum.clear();
	    this->_timeStamp = args.at(5);
	    sum.append(QString("%1-%2/%3%4")
		       .arg(args.at(2)).arg(args.at(3)).arg(args.at(4)).arg(password));
	    sum = QCryptographicHash::hash(sum, QCryptographicHash::Md5);
	    this->_network->sendMessage("auth_ag ext_user none none\n");
	  }
	break;
      }
    case 1:
      {
	const QString hex = sum.toHex();
	QString location(this->_options->locationLineEdit->text());
	QString comment(this->_options->commentLineEdit->text());
	if (location.isEmpty() || location.contains("%L"))
	  this->_network->resolveLocation(location);
	if (comment.isEmpty())
	  comment = "-=[QNetSoul]=-";
	QByteArray	message;
	message.append("ext_user_log ");
	message.append(this->_options->loginLineEdit->text() + ' ');
	message.append(hex);
	message.append(' ');
	message.append(url_encode(location.toStdString().c_str()));
	message.append(' ');
	message.append(url_encode(comment.toStdString().c_str()));
	message.append('\n');
	// DEBUG
	//std::cerr << message.data() << std::endl;
	this->_network->sendMessage(message);
	break;
      }
    case 2:
      {
	QByteArray	state;
	QDateTime	dt = QDateTime::currentDateTime();

	state.append("state actif:");
	state.append(QString::number(static_cast<uint>(dt.toTime_t())));
	state.append("\n");
	this->_network->sendMessage(state);
	//this->_network->sendMessage("ping\n");
	watchLogContacts();
	refreshContacts();
	this->statusbar->showMessage(tr("You are now netsouled."), 2000);
	break;
      }
    case -1: // Network error => reset all contacts
      {
	resetAllContacts();
	break;
      }
    default:;
    }
}

void	QNetsoul::transmitMsg(const QString& login, const QString& msg)
{
  Chat*	chat = getChat(login);

  if (chat)
    {
      chat->insertMessage(this->_options->loginLineEdit->text(), msg, QColor(32, 74, 135));
    }
  QByteArray	result;
  result.append("user_cmd msg_user " + login + " msg ");
  result.append(url_encode(msg.toStdString().c_str()));
  result.append('\n');
  this->_network->sendMessage(result);
}

void	QNetsoul::transmitTypingStatus(const QString& login, bool status)
{
  if (this->_typingNotification == false)
    return;
  QByteArray	cmd("user_cmd msg_user ");

  cmd += login;
  if (!status)
    cmd += " dotnetSoul_UserTyping null\n";
  else
    cmd += " dotnetSoul_UserCancelledTyping null\n";

  this->_network->sendMessage(cmd.data());
}

void	QNetsoul::notifyTypingStatus(const QString& login, bool typing)
{
  QHash<QString, Chat*>::iterator    it;
  it = this->_windowsChat.find(login);
  if (it != this->_windowsChat.end())
    {
      it.value()->notifyTypingStatus(login, typing);
    }
}

void	QNetsoul::setPortrait(const QString& login)
{
  const QString	filename = PortraitResolver::buildFilename(login, false);
  QDir		portraits("./portraits");

  if (false == portraits.exists(filename))
    return;

  Chat*	chat = getChat(login);

  if (NULL != chat)
    {
      chat->portraitLabel->setPixmap(QPixmap("./portraits/" + filename));
      chat->setWindowIcon(QIcon("./portraits/" + filename));
    }
  ContactWidget*	cw = getContact(login);
  if (NULL != cw)
    cw->portraitLabel->setPixmap(QPixmap("./portraits/" + filename));
}

void	QNetsoul::aboutQNetSoul(void)
{
  QMessageBox::about(this, "QNetSoul", this->whatsThis());
}

void	QNetsoul::setProxy(const QNetworkProxy& proxy)
{
  this->_vdm.setProxy(proxy);
  this->_pastebin.setProxy(proxy);
  //this->_portraitResolver.setProxy(proxy); // NOTE: proxy attempts downloading pictures.
  this->_cnf.setProxy(proxy);
}

void	QNetsoul::applyChatOptions
(bool exitOnEscape, bool typingNotification, bool smileys)
{
  this->_exitOnEscape = exitOnEscape;
  this->_typingNotification = typingNotification;
  this->_smileys = smileys;

  QHash<QString, Chat*>::iterator	it = this->_windowsChat.begin();
  QHash<QString, Chat*>::iterator	end = this->_windowsChat.end();

  for (; it != end; ++it)
    {
      it.value()->setExitOnEscape(exitOnEscape);
      it.value()->setSmileys(smileys);
    }
}

void	QNetsoul::startBlinking(void)
{
  this->_blinkingTimer.start(1000);
}

void	QNetsoul::stopBlinking(void)
{
  this->_blinkingTimer.stop();
  this->_trayIcon->setIcon(QIcon(":/images/qnetsoul.png"));
}

void	QNetsoul::systemTrayBlinking(void)
{
  static volatile bool	enabled = true;

  if (enabled)
    this->_trayIcon->setIcon(QIcon(":/images/unread.png"));
  else
    this->_trayIcon->setIcon(QIcon());
  enabled = !enabled;
}


void	QNetsoul::configureProxy(void)
{
  if (this->_options->isProxyEnabled())
    {
      const QNetworkProxy	proxy = this->_options->getProxy();
      setProxy(proxy);
    }
}

Chat*	QNetsoul::getChat(const QString& login)
{
  QHash<QString, Chat*>::iterator	it;

  it = this->_windowsChat.find(login);
  if (this->_windowsChat.end() == it)
    return NULL;
  return it.value();
}

bool	QNetsoul::doesThisContactAlreadyExist(const QString& contact) const
{
  if (getContact(contact))
    return true;
  return false;
}

ContactWidget*	QNetsoul::getContact(const QString& login) const
{
  const int	rows = this->_standardItemModel->rowCount();

  for (int i = 0; i < rows; ++i)
    {
      QStandardItem*	standardItem = this->_standardItemModel->item(i);
      if (standardItem)
        {
	  QWidget*		widget = this->contactsTreeView->indexWidget(standardItem->index());
	  ContactWidget*	contactWidget = dynamic_cast<ContactWidget*>(widget);
	  if (contactWidget)
            {
	      if (login == contactWidget->aliasLabel->text())
                {
		  return contactWidget;
                }
            }
        }
    }
  return NULL;
}

QStringList		QNetsoul::getContactLogins(void) const
{
  QStringList	list;
  const int	rows = this->_standardItemModel->rowCount();

  for (int i = 0; i < rows; ++i)
    {
      QStandardItem*	standardItem = this->_standardItemModel->item(i);
      if (standardItem != NULL)
        {
	  QWidget*		widget = this->contactsTreeView->indexWidget(standardItem->index());
	  ContactWidget*	contactWidget = dynamic_cast<ContactWidget*>(widget);
	  if (contactWidget != NULL)
            {
	      list.push_back(contactWidget->getLogin());
            }
        }
    }
  return list;
}

QList<ContactWidget*>		QNetsoul::getContactWidgets(void) const
{
  QList<ContactWidget*>	list;
  const int		rows = this->_standardItemModel->rowCount();

  for (int i = 0; i < rows; ++i)
    {
      QStandardItem*	standardItem = this->_standardItemModel->item(i);
      if (standardItem != NULL)
        {
	  QWidget*		widget(this->contactsTreeView->indexWidget(standardItem->index()));
	  ContactWidget*	contactWidget = dynamic_cast<ContactWidget*>(widget);
	  if (contactWidget != NULL)
            {
	      list.push_back(contactWidget);
            }
        }
    }
  return list;
}

void	QNetsoul::watchLogContacts(void)
{
  const QStringList	list = getContactLogins();
  const int		size = list.size();

  if (size > 0)
    {
      QByteArray	netMsg("user_cmd watch_log_user {");
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

void	QNetsoul::watchLogContact(const QString& contact)
{
  QByteArray	netMsg("user_cmd watch_log_user ");
  netMsg.append(contact + '\n');
  this->_network->sendMessage(netMsg);
}

void	QNetsoul::refreshContact(const QString& contact)
{
  QByteArray	message("user_cmd who ");
  message.append(contact + '\n');
  this->_network->sendMessage(message);
  this->_portraitResolver.addRequest(contact, false);
}

void	QNetsoul::refreshContacts(void)
{
  const QStringList	contacts = getContactLogins();
  const int		size = contacts.size();

  if (size > 0)
    {
      QByteArray	netMsg("user_cmd who {");
      for (int i = 0; i < size; ++i)
        {
	  // When refreshing get pictures Version 0.04
	  this->_portraitResolver.addRequest(contacts.at(i), false);
	  netMsg.append(contacts.at(i));
	  if (i + 1 < size)
	    netMsg.append(',');
        }
      netMsg.append("}\n");
      this->_network->sendMessage(netMsg);
    }
}

void	QNetsoul::resetAllContacts(void)
{
  const QList<ContactWidget*>	contacts = getContactWidgets();

  const int	size = contacts.size();
  for (int i = 0; i < size; ++i)
    {
      contacts[i]->reset();
    }

  QHash<QString, Chat*>::iterator	it = this->_windowsChat.begin();
  QHash<QString, Chat*>::iterator	end = this->_windowsChat.end();

  for (; it != end; ++it)
    {
      it.value()->statusLabel->setPixmap(QPixmap(":/images/offline"));
    }
}

void	QNetsoul::setupTrayIcon(void)
{
  if (NULL == this->_trayIcon && QSystemTrayIcon::isSystemTrayAvailable())
    {
      this->_trayIcon = new QSystemTrayIcon(this);
      QMenu*	trayIconMenu = new QMenu(this);
      QAction*	trayIconActionQuit = new QAction(QIcon(":/images/quit.png"), tr("Quit"), this);
      trayIconMenu->addAction(trayIconActionQuit);
      this->_trayIcon->setContextMenu(trayIconMenu);
      connect(trayIconActionQuit, SIGNAL(triggered()), SLOT(saveStateBeforeQuiting()));
      connect(this->_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	      SLOT(handleClicksOnTrayIcon(QSystemTrayIcon::ActivationReason)));
      this->_trayIcon->setIcon(QIcon(":/images/qnetsoul.png"));
      this->_trayIcon->show();
    }
}

void	QNetsoul::setupStatusButton(void)
{
  this->_statusPushButton = new QPushButton(tr("Offline"));
  this->_statusPushButton->setFlat(true);
  this->_statusPushButton->setToolTip(tr("Get NetSouled !"));
  this->statusbar->addPermanentWidget(_statusPushButton);
  connect(this->_statusPushButton, SIGNAL(clicked()), SLOT(toggleConnection()));
}

void	QNetsoul::readSettings(void)
{
  QSettings	settings("Epitech", "QNetsoul");

  settings.beginGroup("MainWindow");
  resize(settings.value("size", QSize(240, 545)).toSize());
  move(settings.value("pos", QPoint(501, 232)).toPoint());
  settings.endGroup();
}

void	QNetsoul::writeSettings(void)
{
  QSettings	settings("Epitech", "QNetsoul");

  settings.beginGroup("MainWindow");
  settings.setValue("size", size());
  if (this->isVisible())
    settings.setValue("pos", pos());
  else
    settings.setValue("pos", this->_oldPos);
  settings.endGroup();
  this->_options->writeOptionSettings();
}

void	QNetsoul::loadContacts(const QString& fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    {
      this->statusBar()->showMessage(tr("Unable to load Contacts"), 2000);
      return;
    }

  QList<Contact>	list;
  ContactsReader reader;

  list = reader.read(&file);
  addContact(list);
  this->statusBar()->showMessage(tr("Contacts loaded"), 2000);
}

void	QNetsoul::saveContacts(const QString& fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("QXmlStream Contacts"),
			 tr("Cannot write file %1:\n%2.")
			 .arg(fileName)
			 .arg(file.errorString()));
    return;
  }

  ContactsWriter writer(getContactWidgets());
  if (writer.writeFile(&file))
    this->statusBar()->showMessage(tr("Contacts saved"), 2000);
}

void	QNetsoul::connectQNetsoulItems(void)
{
  connect(&this->_blinkingTimer, SIGNAL(timeout()), SLOT(systemTrayBlinking()));
  connect(this->_addContact->addPushButton, SIGNAL(clicked()), SLOT(addContact()));
  connect(&this->_portraitResolver, SIGNAL(downloadedPortrait(const QString&)),
	  SLOT(setPortrait(const QString&)));
}

void	QNetsoul::connectActionsSignals(void)
{
  // QNetsoul
  connect(actionConnect, SIGNAL(triggered()), SLOT(connectToServer()));
  connect(actionDisconnect, SIGNAL(triggered()), SLOT(disconnect()));
  connect(actionQuit, SIGNAL(triggered()), SLOT(saveStateBeforeQuiting()));
  // Account
  connect(actionAdd_a_contact, SIGNAL(triggered()), SLOT(openAddContactDialog()));
  connect(actionRemove_selected_contact, SIGNAL(triggered()), SLOT(removeSelectedContact()));
  connect(actionRefresh, SIGNAL(triggered()), SLOT(refreshContacts()));
  connect(action_Load_contacts, SIGNAL(triggered()), SLOT(loadContacts()));
  connect(actionSave_contacts_as, SIGNAL(triggered()), SLOT(saveContactsAs()));
  connect(actionToggle_sort_contacts, SIGNAL(triggered()), SLOT(toggleSortContacts()));
  connect(actionGet_a_life, SIGNAL(triggered()), &this->_vdm, SLOT(getALife()));
  connect(actionPastebin, SIGNAL(triggered()), &this->_pastebin, SLOT(pastebinIt()));
  // Options
  connect(actionPreferences, SIGNAL(triggered()), SLOT(openOptionsDialog()));
  // Help
  connect(actionAbout_QNetSoul, SIGNAL(triggered()), SLOT(aboutQNetSoul()));
  connect(actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  // Status
  connect(statusComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sendStatus(const int&)));

  // From Option widget
  connect(this->_options, SIGNAL(loginPasswordFilled()), SLOT(connectToServer()));
  connect(this->_options, SIGNAL(resetProxy(const QNetworkProxy&)),
	  SLOT(setProxy(const QNetworkProxy&)));
  connect(this->_options, SIGNAL(chatOptionsChanged(bool, bool, bool)),
	  SLOT(applyChatOptions(bool, bool, bool)));
}

void	QNetsoul::connectNetworkSignals(void)
{
  connect(this->_network, SIGNAL(handShaking(int, QStringList)),
	  SLOT(processHandShaking(int, QStringList)));
  connect(this->_network, SIGNAL(message(const QString&, const QString&)),
	  SLOT(showConversation(const QString&, const QString&)));
  connect(this->_network, SIGNAL(status(const QString&, const QString&, const QString&)),
	  SLOT(changeStatus(const QString&, const QString&, const QString&)));
  connect(this->_network, SIGNAL(who(const QStringList&)),
	  SLOT(updateContact(const QStringList&)));
  connect(this->_network, SIGNAL(typingStatus(const QString&, bool)),
	  SLOT(notifyTypingStatus(const QString&, bool)));
  connect(&this->_vdm, SIGNAL(sendVdmToQNetsoul(const QString&)),
	  SLOT(showVdmInBalloon(const QString&)));
}

Chat*	QNetsoul::createWindowChat(const QString& name)
{
  Chat*	chat = new Chat(name, this->_exitOnEscape, this->_smileys);
  this->_windowsChat.insert(name, chat);
  connect(chat, SIGNAL(msgToSend(const QString& , const QString&)), this, SLOT(transmitMsg(const QString&, const QString&)));
  connect(chat, SIGNAL(typingSignal(const QString&, bool)), this, SLOT(transmitTypingStatus(const QString&, bool)));
  return chat;
}

void	QNetsoul::deleteAllWindowChats(void)
{
  QHash<QString, Chat*>::const_iterator    cit = this->_windowsChat.constBegin();

  for (; cit != this->_windowsChat.constEnd(); ++cit)
    {
      delete cit.value();
    }
}
