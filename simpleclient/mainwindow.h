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

private:
    Ui::MainWindow *ui;
    QTOpenZwave *m_openzwave;
    QTOZWManager *m_qtozwmanager;
    QString m_serialPort;
};

#endif // MAINWINDOW_H
