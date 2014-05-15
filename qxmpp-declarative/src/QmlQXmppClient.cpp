/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 GameNet
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QtDeclarative/QDeclarativeEngine>

#include <QXmppMessage.h>
#include <QXmppPresence.h>
#include <QXmppArchiveManager.h>
#include <QXmppVCardManager.h>

#include <QmlQXmppPlugin_global.h>
#include <QmlQXmppClient.h>
#include <QmlQXmppMessage.h>
#include <QmlQXmppPresence.h>
#include <QmlQXmppConfiguration.h>
#include <QmlQXmppArchiveManager.h>
#include <QmlQXmppRosterManager.h>
#include <QmlQXmppVCardManager.h>

QmlQXmppClient::QmlQXmppClient(QObject *parent)
    : QObject(parent)
    , _archiveManager(0)
    , _archiveManagerWrapper(0)
    , _rosterManagerWrapper(0)
    , _vcardManagerWrapper(0)
{
  connectSignals();
}

QmlQXmppClient::~QmlQXmppClient()
{
}

QmlQXmppConfiguration *QmlQXmppClient::configuration()
{
  if (!this->_configurationWrapper) {
    this->_configurationWrapper = new QmlQXmppConfiguration(&this->_configuration);
  }
  return this->_configurationWrapper;
}

QmlQXmppArchiveManager *QmlQXmppClient::archiveManager()
{
  if (!this->_archiveManager) {
    this->_archiveManager = new QXmppArchiveManager;
    this->_client.addExtension(this->_archiveManager);
  }

  //if (!this->_archiveManagerWrapper)
  //  this->_archiveManagerWrapper = new QmlQXmppArchiveManager(&this->_client.arcManager(), this);

  return _archiveManagerWrapper;
}

//
QmlQXmppRosterManager *QmlQXmppClient::rosterManager()
{
  if (!this->_rosterManagerWrapper)
    this->_rosterManagerWrapper = new QmlQXmppRosterManager(&this->_client.rosterManager(), this);

  return this->_rosterManagerWrapper;
}

QmlQXmppVCardManager *QmlQXmppClient::vcardManager()
{
  if (!this->_vcardManagerWrapper) {
    //  removing QXmpp vCard manager implementation
    QXmppClientExtension *extension = this->_client.findExtension<QXmppVCardManager>();
    this->_client.removeExtension(extension);

    this->_vcardManagerWrapper = new QmlQXmppVCardManager;
    this->_client.addExtension(this->_vcardManagerWrapper);
  }
 
  return this->_vcardManagerWrapper;
}

QString QmlQXmppClient::clientStatusType()
{
  if (this->_client.clientPresence().type() == QXmppPresence::Available)
    return QmlQXmppPresence::statusToString(this->_client.clientPresence().availableStatusType());
  
  return "offline";
}

void QmlQXmppClient::setClientStatusType(const QString &value)
{
  if (value != this->clientStatusType()) {
    QXmppPresence presence = this->_client.clientPresence();
    if (value == "offline") {
      presence.setType(QXmppPresence::Unavailable);
    } else {
      presence.setType(QXmppPresence::Available);
      presence.setAvailableStatusType(QmlQXmppPresence::stringToStatus(value));
    }
    this->_client.setClientPresence(presence);

    emit this->statusTypeChanged(value);
  }
}

QString QmlQXmppClient::clientStatusText()
{
  return this->_client.clientPresence().statusText();
}

void QmlQXmppClient::setClientStatusText(const QString &value)
{
  if (value != this->clientStatusText()) {
    QXmppPresence presence = this->_client.clientPresence();
    presence.setStatusText(value);
    this->_client.setClientPresence(presence);

    emit this->statusTextChanged(value);
  }
}

void QmlQXmppClient::connectUsingConfiguration()
{
  this->_client.connectToServer(this->_configuration);
}

void QmlQXmppClient::connectToServer(const QString &jid, const QString &password)
{
 this->_client.connectToServer(jid, password);
}

void QmlQXmppClient::disconnectFromServer()
{
 this->_client.disconnectFromServer();
}

void QmlQXmppClient::sendMessage(const QString& bareJid, QVariantMap map)
{
  QXmppMessage msg;

  if (map.contains(QString("body"))) {
    msg.setBody(map["body"].toString());
  }

  if (map.contains(QString("type"))) {
    int type = map["type"].toInt();
    switch (type) {
    case QXmppMessage::Error:
      msg.setType(QXmppMessage::Error);
      break;
    case QXmppMessage::Normal:
      msg.setType(QXmppMessage::Normal);
      break;
    case QXmppMessage::Chat:
      msg.setType(QXmppMessage::Chat);
      break;
    case QXmppMessage::GroupChat:
      msg.setType(QXmppMessage::GroupChat);
      break;
    case QXmppMessage::Headline:
      msg.setType(QXmppMessage::Headline);
      break;
    };
  }

  if (map.contains(QString("state"))) {
    int state = map["state"].toInt();
    switch (state) {
    case QXmppMessage::None:
      msg.setState(QXmppMessage::None);
      break; 
    case QXmppMessage::Active:
      msg.setState(QXmppMessage::Active);
      break;
    case QXmppMessage::Inactive:
      msg.setState(QXmppMessage::Inactive);
      break;
    case QXmppMessage::Gone:
      msg.setState(QXmppMessage::Gone);
      break;
    case QXmppMessage::Composing:
      msg.setState(QXmppMessage::Composing);
      break;
    case QXmppMessage::Paused:
      msg.setState(QXmppMessage::Paused);
      break;
    }
  }

  if (map.contains(QString("attentionRequest"))) {
    bool attentionRequested = map["attentionRequest"].toBool();
    msg.setAttentionRequested(attentionRequested);
  }

  QStringList resources = this->_client.rosterManager().getResources(bareJid);
  if (!resources.isEmpty()) {
    for (int i = 0; i < resources.size(); ++i) {
      msg.setTo(bareJid + "/" + resources.at(i));
      this->_client.sendPacket(msg);
    }
  } else {
    msg.setTo(bareJid);
    this->_client.sendPacket(msg);
  }
}

void QmlQXmppClient::onMessageReceived(const QXmppMessage& message)
{
  QmlQXmppMessage qmlmessage(message);
  emit messageReceived(&qmlmessage);
}

void QmlQXmppClient::onPresenceReceived(const QXmppPresence &presence)
{
  QmlQXmppPresence presenceWrapper(presence);
  emit this->presenceReceived(&presenceWrapper);
}

void QmlQXmppClient::connectSignals()
{
  SIGNAL_CONNECT_CHECK(connect(&this->_client, SIGNAL(error(QXmppClient::Error)), this, SIGNAL(error(QXmppClient::Error))));
  SIGNAL_CONNECT_CHECK(connect(&this->_client, SIGNAL(connected()), this, SIGNAL(connected())));
  SIGNAL_CONNECT_CHECK(connect(&this->_client, SIGNAL(disconnected()), this, SIGNAL(disconnected())));
  SIGNAL_CONNECT_CHECK(connect(&this->_client, SIGNAL(messageReceived(const QXmppMessage &)), this, SLOT(onMessageReceived(const QXmppMessage &))));
  SIGNAL_CONNECT_CHECK(connect(&this->_client, SIGNAL(presenceReceived(const QXmppPresence &)), this, SLOT(onPresenceReceived(const QXmppPresence &))));
}