/****************************************************************************
**
** Copyright (C) 2019 Ford Motor Company
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtRemoteObjects module of the Qt Toolkit.
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
#include "qtozw_logging.h"

#include <QWebSocket>

WebSocketIoDevice::WebSocketIoDevice(QWebSocket *webSocket, bool client, QString auth, QObject *parent) :
    QIODevice(parent),
    m_socket(webSocket),
    m_client(client),
    m_state(Stage_None),
    m_auth(auth),
    m_version(0),
    m_authattempts(0)
{
    open(QIODevice::ReadWrite);
    connect(webSocket, &QWebSocket::aboutToClose, this, &WebSocketIoDevice::aboutToClose);
    connect(webSocket, &QWebSocket::disconnected, this, &WebSocketIoDevice::disconnected);
    connect(webSocket, &QWebSocket::textMessageReceived, this, &WebSocketIoDevice::processTextMessage);
    if (!m_client) {
        /* if we are the server, send our Protocol Message */
        /* PROTOCOL <VERSION>*/
        QString msg("PROTOCOL %1");
        webSocket->sendTextMessage(msg.arg(PROTOCOL_VERSION));
        m_state = Stage_Protocol;
    }
}

void WebSocketIoDevice::processTextMessage(const QString &msg) {
    /* State Machine for Client */
    if (msg.section(" ", 0, 0) == "ERROR") {
        qCWarning(qtrowebsocket) << "Got Error Message From Peer: " << msg;
        emit this->authError(msg);
        m_socket->close(QWebSocketProtocol::CloseCodeNormal);
        return;
    } 
    if (m_client) { 
        switch (m_state) {
            case Stage_None: {
                /* we are expecting a PROTOCOL Message */
                QString Cmd = msg.section(" ", 0, 0);
                if (Cmd != "PROTOCOL") {
                    qCWarning(qtrowebsocket) << "Got Invalid Command " << msg << " In Stage_None";
                    m_socket->sendTextMessage("ERROR Protocol Violation");
                    emit this->authError(QString("Got Invalid Command %1 In Stage_None").arg(msg));
                    m_socket->close(QWebSocketProtocol::CloseCodeProtocolError);
                    return;
                }
                m_version = msg.section(" ", 1, 1).toInt();
                if (m_version != PROTOCOL_VERSION) {
                    qCWarning(qtrowebsocket) << "Got Invalid Protocol Version " << m_version;
                    m_socket->sendTextMessage("ERROR Incompatible Protocol Version");
                    emit this->authError(QString("Got Invalid Protocol Version"));
                    m_socket->close(QWebSocketProtocol::CloseCodeProtocolError);
                }
                /* future Vesions - Check Version */
                m_socket->sendTextMessage("PROTOCOL OK");
                m_state = Stage_Authenticate;
                break;
            }
            case Stage_Authenticate: {
                QString Cmd = msg.section(" ", 0, 0);
                if (Cmd == "AUTH") {
                    QString auth("AUTH ");
                    if (!m_auth.isEmpty()) {
                        auth = auth.append(m_auth);
                    }                      
                    m_socket->sendTextMessage(auth);
                } else if (Cmd == "START") {
                    m_state = Stage_Authenticated;
                    connect(m_socket, &QWebSocket::binaryMessageReceived, this, [this](const QByteArray &message){
                        m_buffer.append(message);
                        emit readyRead();
                    });
                    connect(m_socket, &QWebSocket::bytesWritten, this, &WebSocketIoDevice::bytesWritten);
                    emit this->authenticated();                        
                } else {
                    m_socket->sendTextMessage("ERROR Protocol Violation");
                    qCWarning(qtrowebsocket) << "Got Invalid Command " << msg << " In Stage_Authenticate";
                    emit this->authError(QString("Got Invalid Command %1 In Stage_Authenticate").arg(msg));
                    m_socket->close(QWebSocketProtocol::CloseCodeProtocolError);
                }
                break;
            }
            case Stage_Protocol:
            case Stage_Authenticated:
            {
                qCWarning(qtrowebsocket) << "Invalid Stage in Client Connection Negiation";
                m_socket->sendTextMessage("ERROR Protocol Violation");
                emit this->authError(QString("Got Invalid Command %1 In Stage_Authenticate").arg(msg));
                m_socket->close(QWebSocketProtocol::CloseCodeProtocolError);
                break;
            }
        }
    } else {
        /* State Machine for Server */
        switch (m_state) {
            case Stage_Protocol: {
                QString Cmd = msg.section(" ", 0, 0);
                if (Cmd == "PROTOCOL") {
                    if (msg.section(" ", 1, 1) == "OK") {
                        m_socket->sendTextMessage("AUTH");
                        m_state = Stage_Authenticate;
                    } else {
                        qCWarning(qtrowebsocket) << "Protocol Negiation Failed... " << msg;
                        emit this->authError(QString("Protocol Negiation Failed... %1").arg(msg));
                        m_socket->sendTextMessage("ERROR Protocol Violation");
                        m_socket->close(QWebSocketProtocol::CloseCodeProtocolError);
                        return;
                    }
                } else {
                    m_socket->sendTextMessage("ERROR Protocol Violation");
                    qCWarning(qtrowebsocket) << "Got Invalid Command " << msg << " In Stage_Protocol";
                    emit this->authError(QString("Got Invalid Command %1 In Stage_Protocol").arg(msg));
                    m_socket->close(QWebSocketProtocol::CloseCodeProtocolError);
                }
                break;
            }
            case Stage_Authenticate: {
                QString Cmd = msg.section(" ", 0, 0);
                if (Cmd == "AUTH") {
                    if (!m_auth.isEmpty()) { 
                        if (msg.section(" ", 1, 1) == m_auth) {
                            m_socket->sendTextMessage("START");
                            m_state = Stage_Authenticated;
                            connect(m_socket, &QWebSocket::binaryMessageReceived, this, [this](const QByteArray &message){
                                m_buffer.append(message);
                                emit readyRead();
                            });
                            connect(m_socket, &QWebSocket::bytesWritten, this, &WebSocketIoDevice::bytesWritten);
                            emit this->authenticated();                        
                        } else {
                            /* three attempts, starting at 0 */
                            if (m_authattempts < 2) {
                                m_socket->sendTextMessage("AUTH");
                                m_authattempts++;
                            } else {
                                qCWarning(qtrowebsocket) << "Closing WebSocket Server as Failed Auth Attempts";
                                emit this->authError(QString("Exceeded Authentication Attempts %1").arg(m_authattempts+1));
                                m_socket->sendTextMessage("ERROR Too Many Authentication Attempts");
                                m_socket->close(QWebSocketProtocol::CloseCodeProtocolError);
                            }
                        }
                    } else { 
                        /* no Authentication is required */
                        m_socket->sendTextMessage("START");
                        m_state = Stage_Authenticated;
                        connect(m_socket, &QWebSocket::binaryMessageReceived, this, [this](const QByteArray &message){
                            m_buffer.append(message);
                            emit readyRead();
                        });
                        connect(m_socket, &QWebSocket::bytesWritten, this, &WebSocketIoDevice::bytesWritten);
                        emit this->authenticated();                        
                    }
                } else {
                    m_socket->sendTextMessage("ERROR Protocol Violation");
                    qCWarning(qtrowebsocket) << "Got Invalid Command " << msg << " In Stage_Protocol";
                    emit this->authError(QString("Got Invalid Command %1 In Stage_Autheticate").arg(msg));
                    m_socket->close(QWebSocketProtocol::CloseCodeProtocolError);
                }
                break;
            }
            case Stage_None:
            case Stage_Authenticated:
            {
                qCWarning(qtrowebsocket) << "Invalid Stage in Client Connection Negiation";
                m_socket->sendTextMessage("ERROR Protocol Violation");
                emit this->authError(QString("Got Invalid Command %1 In Stage_Authenticate").arg(msg));
                m_socket->close(QWebSocketProtocol::CloseCodeProtocolError);
                break;
            }

        }
    }
}
qint64 WebSocketIoDevice::bytesAvailable() const
{
    return QIODevice::bytesAvailable() + m_buffer.size();
}

bool WebSocketIoDevice::isSequential() const
{
    return true;
}

void WebSocketIoDevice::close()
{
    if (m_socket)
        m_socket->close();
}

qint64 WebSocketIoDevice::readData(char *data, qint64 maxlen)
{
    auto sz = std::min(int(maxlen), m_buffer.size());
    if (sz <= 0)
        return sz;
    memcpy(data, m_buffer.constData(), size_t(sz));
    m_buffer.remove(0, sz);
    return sz;
}

qint64 WebSocketIoDevice::writeData(const char *data, qint64 len)
{
    if (m_socket)
        return m_socket->sendBinaryMessage(QByteArray{data, int(len)});
    return -1;
}
