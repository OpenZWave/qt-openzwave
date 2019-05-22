/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "websocketiodevice.h"

#include <QtCore/QDebug>

void WebSocketIODevice::print() {
    qDebug() << m_socket->errorString();
    qDebug() << m_socket->state();
}
WebSocketIODevice::WebSocketIODevice(QObject *parent)
    : QIODevice(parent)
{
    m_socket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(m_socket, &QWebSocket::connected, this, &WebSocketIODevice::onSocketConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &WebSocketIODevice::onSocketDisconnected);
    connect(m_socket, &QWebSocket::binaryMessageReceived, this, &WebSocketIODevice::handleBinaryMessage);
    connect(m_socket, &QWebSocket::stateChanged, this, &WebSocketIODevice::print);
    connect(m_socket, &QWebSocket::aboutToClose, this, &WebSocketIODevice::onAboutToClose);
    connect(this, &WebSocketIODevice::readyRead, this, &WebSocketIODevice::print);

}

WebSocketIODevice::WebSocketIODevice(QWebSocket *socket, QObject *parent)
    :QIODevice (parent),
    m_socket(socket)
{
    m_socket = socket;
    m_socket->setParent(this);
    connect(m_socket, &QWebSocket::connected, this, &WebSocketIODevice::onSocketConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &WebSocketIODevice::onSocketDisconnected);
    connect(m_socket, &QWebSocket::binaryMessageReceived, this, &WebSocketIODevice::handleBinaryMessage);
    connect(m_socket, &QWebSocket::stateChanged, this, &WebSocketIODevice::print);
    connect(m_socket, &QWebSocket::aboutToClose, this, &WebSocketIODevice::onAboutToClose);
}

bool WebSocketIODevice::open(QIODevice::OpenMode mode)
{
    // Cannot use an URL because of missing sub protocol support
    // Have to use QNetworkRequest, see QTBUG-38742
    QNetworkRequest request;
    request.setUrl(m_url);
    request.setRawHeader("Sec-WebSocket-Protocol", m_protocol.constData());
    qDebug() << m_url;
    m_socket->open(request);
    qDebug() << "Made Request";
    return QIODevice::open(mode);
}

void WebSocketIODevice::close()
{
    m_socket->close();
    QIODevice::close();
}

qint64 WebSocketIODevice::readData(char *data, qint64 maxlen)
{
    qint64 bytesToRead = qMin(maxlen, (qint64)m_buffer.size());
    memcpy(data, m_buffer.constData(), bytesToRead);
    m_buffer = m_buffer.right(m_buffer.size() - bytesToRead);
    qDebug() << "readData";
    return bytesToRead;
}

qint64 WebSocketIODevice::writeData(const char *data, qint64 len)
{
    QByteArray msg(data, len);
    const int length = m_socket->sendBinaryMessage(msg);
 qDebug() << "writeData " << msg;
    return length;
}

void WebSocketIODevice::setUrl(const QUrl &url)
{
    m_url = url;
}

void WebSocketIODevice::setProtocol(const QByteArray &data)
{
    m_protocol = data;
}

void WebSocketIODevice::handleBinaryMessage(const QByteArray &msg)
{
    m_buffer.append(msg);
    qDebug() << "binaryMessage" << msg;
    emit readyRead();
}

void WebSocketIODevice::onSocketConnected()
{
    emit connected();
}

void WebSocketIODevice::onSocketDisconnected()
{
    emit disconnected();
}

void WebSocketIODevice::onAboutToClose()
{
    emit aboutToClose();
}



WebSocketServer::WebSocketServer(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("OpenZWave"),
                      QWebSocketServer::NonSecureMode, this))
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        qDebug() << "WebSocketServer "<< m_pWebSocketServer->serverName() << " listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &WebSocketServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &WebSocketServer::closed);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, [this]() {
            qDebug() << "WebSocket closed.";
        });
        connect(m_pWebSocketServer, &QWebSocketServer::serverError, this, [this]() {
            qDebug() << "WebSocket Error.";
        });
        connect(m_pWebSocketServer, &QWebSocketServer::acceptError, this, [this]() {
            qDebug() << "WebSocket acceptError.";
        });
        connect(m_pWebSocketServer, &QWebSocketServer::acceptError, this, [this]() {
            qDebug() << "WebSocket acceptError.";
        });


    }
}

WebSocketServer::~WebSocketServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void WebSocketServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
    m_clients << new WebSocketIODevice(pSocket, this);
    qDebug() << "New Connection" << pSocket;
    emit newConnection();
}

WebSocketIODevice *WebSocketServer::nextPendingConnection()
{
    if (!m_clients.isEmpty())
        return m_clients.takeLast();

    qDebug() << "nextPendingConnection called with Empty List";
    return nullptr;
}
