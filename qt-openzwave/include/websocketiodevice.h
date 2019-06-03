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

#ifndef WEBSOCKETIODEVICE_H
#define WEBSOCKETIODEVICE_H

#include <QtCore/QIODevice>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

class WebSocketIODevice : public QIODevice
{
    Q_OBJECT
public:
    WebSocketIODevice(QObject *parent = nullptr);
    WebSocketIODevice(QWebSocket *socket, QObject *parent = nullptr);

    bool open(OpenMode mode) override;
    void close() override;
    void print();
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 write(const char *data, qint64 len) {return writeData(data, len);}

    void setUrl(const QUrl &url);
    void setProtocol(const QByteArray &data);
    bool isOpen() const { qDebug() << "IsOpen" << QIODevice::isOpen(); return QIODevice::isOpen(); }
Q_SIGNALS:
    void connected();
    void disconnected();
    void aboutToClose();
    void readyRead();

public slots:
    void handleBinaryMessage(const QByteArray &msg);
private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onAboutToClose();

private:
    QByteArray m_protocol;
    QByteArray m_buffer;
    QWebSocket *m_socket;
    QUrl m_url;
};


class WebSocketServer : public QObject
{
    Q_OBJECT
public:
    WebSocketServer(quint16 port, QObject *parent = nullptr);
    ~WebSocketServer();

    WebSocketIODevice *nextPendingConnection();

Q_SIGNALS:
    void closed();
    void newConnection();

private Q_SLOTS:
    void onNewConnection();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<WebSocketIODevice *> m_clients;
};


#endif // WEBSOCKETIODEVICE_H
