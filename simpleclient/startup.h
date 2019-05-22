#ifndef STARTUP_H
#define STARTUP_H

#include <QDialog>
#include <QObject>


namespace Ui {
class Startup;
}

class Startup : public QDialog
{
    Q_OBJECT

public Q_SLOTS:
    void localPressed();
    void remotePressed();

Q_SIGNALS:
    void startLocal(QString, bool);
    void startRemote(QString);

public:
    explicit Startup(QWidget *parent = nullptr);
    ~Startup();


private:
    Ui::Startup *ui;
};

#endif // STARTUP_H
