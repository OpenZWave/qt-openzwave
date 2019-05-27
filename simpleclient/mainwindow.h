#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qtopenzwave.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void start();

public Q_SLOTS:
    void startRemote(QString);
    void startLocal(QString, bool);
    void QTOZW_Ready();
    void valueAdded(uint64_t vidKey);
    void valueRemoved(uint64_t vidKey);
    void valueChanged(uint64_t vidKey);
    void valueRefreshed(uint64_t vidKey);
    void nodeNew(uint8_t node);
    void nodeAdded(uint8_t node);
    void nodeRemoved(uint8_t node);
    void nodeReset(uint8_t node);
    void nodeNaming(uint8_t node);
    void nodeEvent(uint8_t node, uint8_t event);
    void nodeProtocolInfo(uint8_t node);
    void nodeEssentialNodeQueriesComplete(uint8_t node);
    void nodeQueriesComplete(uint8_t node);
    void driverReady(uint32_t homeID);
    void driverFailed(uint32_t homeID);
    void driverReset(uint32_t homeID);
    void driverRemoved(uint32_t homeID);
    void driverAllNodesQueriedSomeDead();
    void driverAllNodesQueried();
    void driverAwakeNodesQueried();
    void controllerCommand(uint8_t command);
//    void ozwNotification(OpenZWave::Notification::NotificationCode event);
// void ozwUserAlert(OpenZWave::Notification::UserAlertNotification event);
    void manufacturerSpecificDBReady();

    void starting();
    void started(uint32_t homeID);
    void stopped(uint32_t homeID);
//    void error(QTOZWErrorCodes errorcode);



private:
    Ui::MainWindow *ui;
    QTOpenZwave *m_openzwave;
    QTOZWManager *m_qtozwmanager;
    QString m_serialPort;
};

#endif // MAINWINDOW_H
