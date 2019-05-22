#ifndef QTOPENZWAVE_H
#define QTOPENZWAVE_H

#include <QObject>
#include <QtRemoteObjects>
#include "qt-openzwave_global.h"
#include "qtozwmanager.h"
#include "websocketiodevice.h"



class QTOPENZWAVESHARED_EXPORT QTOpenZwave : public QObject
{
    Q_OBJECT
public:
    QTOpenZwave(QObject *parent = nullptr);
    QTOZWManager *GetManager();

private Q_SLOT:
    void newWSConnection();

private:
    QTOZWManager *m_manager;
};

#endif // QTOPENZWAVE_H
