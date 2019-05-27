#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "startup.h"
#include "qtozw_itemdelegate.h"
#include <qtozwproxymodels.h>

#define CONNECTSIGNALS(x) QObject::connect(this->m_qtozwmanager, &QTOZWManager::x, this, &MainWindow::x)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->m_openzwave = new QTOpenZwave(this);
    this->m_qtozwmanager = this->m_openzwave->GetManager();
    QObject::connect(this->m_qtozwmanager, &QTOZWManager::ready, this, &MainWindow::QTOZW_Ready);
    CONNECTSIGNALS(valueAdded);
    CONNECTSIGNALS(valueRemoved);
    CONNECTSIGNALS(valueChanged);
    CONNECTSIGNALS(valueRefreshed);
    CONNECTSIGNALS(nodeNew);
    CONNECTSIGNALS(nodeAdded);
    CONNECTSIGNALS(nodeRemoved);
    CONNECTSIGNALS(nodeReset);
    CONNECTSIGNALS(nodeNaming);
    CONNECTSIGNALS(nodeEvent);
    CONNECTSIGNALS(nodeProtocolInfo);
    CONNECTSIGNALS(nodeEssentialNodeQueriesComplete);
    CONNECTSIGNALS(nodeQueriesComplete);
    CONNECTSIGNALS(driverReady);
    CONNECTSIGNALS(driverFailed);
    CONNECTSIGNALS(driverReset);
    CONNECTSIGNALS(driverRemoved);
    CONNECTSIGNALS(driverAllNodesQueriedSomeDead);
    CONNECTSIGNALS(driverAwakeNodesQueried);
    CONNECTSIGNALS(driverAllNodesQueried);
    CONNECTSIGNALS(controllerCommand);
    CONNECTSIGNALS(manufacturerSpecificDBReady);
    CONNECTSIGNALS(starting);
    CONNECTSIGNALS(started);
    CONNECTSIGNALS(stopped);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start() {
    Startup win(this);
    win.setModal(true);
    QObject::connect(&win, &Startup::startLocal, this, &MainWindow::startLocal);
    QObject::connect(&win, &Startup::startRemote, this, &MainWindow::startRemote);
    win.exec();
}

void MainWindow::startRemote(QString url) {
    QUrl server;
    server.setHost(url);
    server.setPort(1983);
    server.setScheme("tcp");
    qDebug() << server;
    this->m_qtozwmanager->initilizeReplica(server);

}
void MainWindow::startLocal(QString serialPort, bool startServer) {
    this->m_serialPort = serialPort;
    this->m_qtozwmanager->initilizeSource(startServer);
    qDebug() << "startLocal: " << this->m_serialPort;
}

void MainWindow::QTOZW_Ready() {
    this->ui->statusbar->showMessage("OpenZWave Ready");
    qDebug() << "OZW Ready " << this->m_serialPort;
    if (this->m_qtozwmanager->isRunning() == false) {
        this->m_qtozwmanager->open(this->m_serialPort);
    }
    QTOZW_proxyNodeModel *proxyNodeModel = new QTOZW_proxyNodeModel(this);
    proxyNodeModel->setSourceModel(this->m_qtozwmanager->getNodeModel());
    this->ui->nodeView->setModel(proxyNodeModel);
    this->ui->nodeView->setSortingEnabled(true);
    this->ui->nodeView->horizontalHeader()->setSectionsMovable(true);
    this->ui->nodeView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->nodeView->verticalHeader()->hide();
    this->ui->nodeView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->nodeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->nodeView->setSelectionMode(QAbstractItemView::SingleSelection);
    QItemSelectionModel *selectNodeModel = this->ui->nodeView->selectionModel();


    QTOZW_proxyValueModel *proxyUserValueModel = new QTOZW_proxyValueModel(this);
    proxyUserValueModel->setSourceModel(this->m_qtozwmanager->getValueModel());
    proxyUserValueModel->setSelectionModel(selectNodeModel);
    proxyUserValueModel->setFilterGenre(QTOZW_ValueIds::ValueIdGenres::User);

    QTOZW_ItemDelegate *delegate = new QTOZW_ItemDelegate(this);
    this->ui->userView->setItemDelegateForColumn(QTOZW_ValueIds::ValueIdColumns::Value, delegate);
    this->ui->userView->setModel(proxyUserValueModel);
    this->ui->userView->setSortingEnabled(true);
    this->ui->userView->horizontalHeader()->setSectionsMovable(true);
    this->ui->userView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->userView->verticalHeader()->hide();
    this->ui->userView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->userView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->userView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->userView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    this->ui->userView->setFrameShape(QFrame::NoFrame);



    QTOZW_proxyValueModel *proxyConfigValueModel = new QTOZW_proxyValueModel(this);
    proxyConfigValueModel->setSourceModel(this->m_qtozwmanager->getValueModel());
    proxyConfigValueModel->setSelectionModel(selectNodeModel);
    proxyConfigValueModel->setFilterGenre(QTOZW_ValueIds::ValueIdGenres::Config);
    this->ui->configView->setModel(proxyConfigValueModel);
    this->ui->configView->setSortingEnabled(true);
    this->ui->configView->horizontalHeader()->setSectionsMovable(true);
    this->ui->configView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->configView->verticalHeader()->hide();
    this->ui->configView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->configView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->configView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->configView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    this->ui->configView->setFrameShape(QFrame::NoFrame);

    this->ui->configView->setItemDelegateForColumn(QTOZW_ValueIds::ValueIdColumns::Value, delegate);


    QTOZW_proxyValueModel *proxySystemValueModel = new QTOZW_proxyValueModel(this);
    proxySystemValueModel->setSourceModel(this->m_qtozwmanager->getValueModel());
    proxySystemValueModel->setSelectionModel(selectNodeModel);
    proxySystemValueModel->setFilterGenre(QTOZW_ValueIds::ValueIdGenres::System);
    this->ui->systemView->setModel(proxySystemValueModel);
    this->ui->systemView->setSortingEnabled(true);
    this->ui->systemView->horizontalHeader()->setSectionsMovable(true);
    this->ui->systemView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->systemView->verticalHeader()->hide();
    this->ui->systemView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->systemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->systemView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->systemView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    this->ui->systemView->setFrameShape(QFrame::NoFrame);


    this->ui->systemView->setItemDelegateForColumn(QTOZW_ValueIds::ValueIdColumns::Value, delegate);


    QTOZW_proxyAssociationModel *proxyAssociationModel = new QTOZW_proxyAssociationModel(this);
    proxyAssociationModel->setSourceModel(this->m_qtozwmanager->getAssociationModel());
    proxyAssociationModel->setSelectionModel(selectNodeModel);
    this->ui->AssociationView->setModel(proxyAssociationModel);
    this->ui->AssociationView->setSortingEnabled(true);
    this->ui->AssociationView->horizontalHeader()->setSectionsMovable(true);
    this->ui->AssociationView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->AssociationView->verticalHeader()->hide();
    this->ui->AssociationView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->ui->AssociationView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->AssociationView->setSelectionMode(QAbstractItemView::SingleSelection);

}

void MainWindow::valueAdded(quint64 vidKey) {
    Q_UNUSED(vidKey)
    this->ui->statusbar->showMessage("ValueAdded Notification", 2000);
}
void MainWindow::valueRemoved(quint64 vidKey) {
    Q_UNUSED(vidKey)
    this->ui->statusbar->showMessage("ValueRemoved Notification", 2000);
}
void MainWindow::valueChanged(quint64 vidKey) {
    Q_UNUSED(vidKey)
    this->ui->statusbar->showMessage("ValueChanged Notification", 2000);
}
void MainWindow::valueRefreshed(quint64 vidKey) {
    Q_UNUSED(vidKey)
    this->ui->statusbar->showMessage("ValueRefreshed Notification", 2000);
}
void MainWindow::nodeNew(quint8 node) {
    Q_UNUSED(node);
    this->ui->statusbar->showMessage("NodeNew Notification", 2000);
}
void MainWindow::nodeAdded(quint8 node) {
    Q_UNUSED(node);
    this->ui->statusbar->showMessage("NodeAdded Notification", 2000);
}
void MainWindow::nodeRemoved(quint8 node) {
    Q_UNUSED(node);
    this->ui->statusbar->showMessage("NodeRemoved Notification", 2000);
}
void MainWindow::nodeReset(quint8 node) {
    Q_UNUSED(node);
    this->ui->statusbar->showMessage("NodeReset Notification", 2000);
}
void MainWindow::nodeNaming(quint8 node) {
    Q_UNUSED(node);
    this->ui->statusbar->showMessage("NodeNaming Notification", 2000);
}
void MainWindow::nodeEvent(quint8 node, quint8 event) {
    Q_UNUSED(node)
    Q_UNUSED(event)
    this->ui->statusbar->showMessage("NodeEvent Notification", 2000);
}
void MainWindow::nodeProtocolInfo(quint8 node) {
    Q_UNUSED(node)
    this->ui->statusbar->showMessage("NodeProtocolInfo Notification", 2000);
}
void MainWindow::nodeEssentialNodeQueriesComplete(quint8 node) {
    Q_UNUSED(node)
    this->ui->statusbar->showMessage("NodeEssentialNodeQueriesComplete Notification", 2000);
}
void MainWindow::nodeQueriesComplete(quint8 node) {
    static QMap<quint8, bool> refreshdone;
    this->ui->statusbar->showMessage("nodeQueriesComplete Notification", 2000);
    if (!refreshdone[node]) {
        refreshdone[node] = true;
        if (node != 1)
            this->m_qtozwmanager->requestAllConfigParam(node);
    }
}
void MainWindow::driverReady(quint32 homeID) {
    Q_UNUSED(homeID)
    this->ui->statusbar->showMessage("DriverReady Notification", 2000);
}
void MainWindow::driverFailed(quint32 homeID) {
    Q_UNUSED(homeID)
    this->ui->statusbar->showMessage("DriverFailed Notification", 2000);
}
void MainWindow::driverReset(quint32 homeID) {
    Q_UNUSED(homeID)
    this->ui->statusbar->showMessage("DriverReset Notification", 2000);
}
void MainWindow::driverRemoved(quint32 homeID) {
    Q_UNUSED(homeID)
    this->ui->statusbar->showMessage("DriverRemoved Notification", 2000);
}
void MainWindow::driverAllNodesQueriedSomeDead() {
    this->ui->statusbar->showMessage("DriverAllNodesQueriedSomeDead Notification", 2000);
}
void MainWindow::driverAllNodesQueried() {
    this->ui->statusbar->showMessage("DriverAllNodesQueried Notification", 2000);
}
void MainWindow::driverAwakeNodesQueried() {
    this->ui->statusbar->showMessage("DriverAwakeNodesQueried Notification", 2000);
}
void MainWindow::controllerCommand(quint8 command) {
    Q_UNUSED(command)
    this->ui->statusbar->showMessage("ControllerCommand Notification", 2000);
}
//    void ozwNotification(OpenZWave::Notification::NotificationCode event);
// void ozwUserAlert(OpenZWave::Notification::UserAlertNotification event);
void MainWindow::manufacturerSpecificDBReady() {
    this->ui->statusbar->showMessage("ManufacturerSpecificDBReady Notification", 2000);
}

void MainWindow::starting() {
    this->ui->statusbar->showMessage("Starting", 2000);
}
void MainWindow::started(quint32 homeID) {
    Q_UNUSED(homeID)
    this->ui->statusbar->showMessage("Started", 2000);
}
void MainWindow::stopped(quint32 homeID) {
    Q_UNUSED(homeID)
    this->ui->statusbar->showMessage("Stopped", 2000);
}


