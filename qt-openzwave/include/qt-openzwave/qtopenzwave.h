#ifndef QTOPENZWAVE_H
#define QTOPENZWAVE_H

#include <QObject>
#include <QtRemoteObjects>
#include "qt-openzwave_global.h"
#include "qtozwmanager.h"

class QTOPENZWAVESHARED_EXPORT QTOpenZwave : public QObject
{
    Q_OBJECT
public:
    QTOpenZwave(QObject *parent = nullptr, QDir DBPath = QDir("./config/"), QDir UserPath = QDir("./config/"));
    QTOZWManager *GetManager();

private Q_SLOT:
    void newWSConnection();

private:
    QTOZWManager *m_manager;
    QDir m_ozwdbpath;
    QDir m_ozwuserpath;
};

#endif // QTOPENZWAVE_H
