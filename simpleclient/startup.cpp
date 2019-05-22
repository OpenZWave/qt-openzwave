#include "startup.h"
#include "ui_startup.h"
#include <QMessageBox>


Startup::Startup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Startup)
{
    ui->setupUi(this);

    QObject::connect(ui->startlocal, &QPushButton::clicked, this, &Startup::localPressed);
    QObject::connect(ui->startremote, &QPushButton::clicked, this, &Startup::remotePressed);

}

Startup::~Startup()
{
    delete ui;
}

void Startup::localPressed() {
    QString serialport = ui->serialport->text();
    if (serialport.length() == 0) {
        QMessageBox::critical(this, tr("Error"), tr("You Must Specify a Serial Port"), QMessageBox::Ok);
    }
    emit this->startLocal(serialport, ui->enableserver->isChecked());
    this->close();
}
void Startup::remotePressed() {
    QString remotehost = ui->remotehost->text();
    if (remotehost.length() == 0) {
        QMessageBox::critical(this, tr("Error"), tr("You Must Specify a Remote Host"), QMessageBox::Ok);
    }
    emit this->startRemote(remotehost);
    this->close();
}

