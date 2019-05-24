#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "startup.h"
#include "qtozw_itemdelegate.h"
#include <qtozwproxymodels.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->m_openzwave = new QTOpenZwave(this);
    this->m_qtozwmanager = this->m_openzwave->GetManager();
    QObject::connect(this->m_qtozwmanager, &QTOZWManager::ready, this, &MainWindow::QTOZW_Ready);
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
    qDebug() << "OZW Ready " << this->m_serialPort;
    if (this->m_qtozwmanager->isRunning() == false) {
        this->m_qtozwmanager->open(this->m_serialPort);
    }
    QTOZW_proxyNodeModel *proxyNodeModel = new QTOZW_proxyNodeModel(this);
    proxyNodeModel->setSourceModel(this->m_qtozwmanager->getNodeModel());
    this->ui->nodeView->setModel(proxyNodeModel);
    this->ui->nodeView->setSortingEnabled(true);
    this->ui->nodeView->horizontalHeader()->setSectionsMovable(true);
    this->ui->nodeView->verticalHeader()->hide();
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
    this->ui->userView->verticalHeader()->hide();
    this->ui->userView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->userView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->userView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    QTOZW_proxyValueModel *proxyConfigValueModel = new QTOZW_proxyValueModel(this);
    proxyConfigValueModel->setSourceModel(this->m_qtozwmanager->getValueModel());
    proxyConfigValueModel->setSelectionModel(selectNodeModel);
    proxyConfigValueModel->setFilterGenre(QTOZW_ValueIds::ValueIdGenres::Config);
    this->ui->configView->setModel(proxyConfigValueModel);
    this->ui->configView->setSortingEnabled(true);
    this->ui->configView->horizontalHeader()->setSectionsMovable(true);
    this->ui->configView->verticalHeader()->hide();
    this->ui->configView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->configView->setSelectionMode(QAbstractItemView::SingleSelection);

    this->ui->configView->setItemDelegateForColumn(QTOZW_ValueIds::ValueIdColumns::Value, delegate);


    QTOZW_proxyValueModel *proxySystemValueModel = new QTOZW_proxyValueModel(this);
    proxySystemValueModel->setSourceModel(this->m_qtozwmanager->getValueModel());
    proxySystemValueModel->setSelectionModel(selectNodeModel);
    proxySystemValueModel->setFilterGenre(QTOZW_ValueIds::ValueIdGenres::System);
    this->ui->systemView->setModel(proxySystemValueModel);
    this->ui->systemView->setSortingEnabled(true);
    this->ui->systemView->horizontalHeader()->setSectionsMovable(true);
    this->ui->systemView->verticalHeader()->hide();
    this->ui->systemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->systemView->setSelectionMode(QAbstractItemView::SingleSelection);

    this->ui->systemView->setItemDelegateForColumn(QTOZW_ValueIds::ValueIdColumns::Value, delegate);


    QTOZW_proxyAssociationModel *proxyAssociationModel = new QTOZW_proxyAssociationModel(this);
    proxyAssociationModel->setSourceModel(this->m_qtozwmanager->getAssociationModel());
    proxyAssociationModel->setSelectionModel(selectNodeModel);
    this->ui->AssociationView->setModel(proxyAssociationModel);
    this->ui->AssociationView->setSortingEnabled(true);
    this->ui->AssociationView->horizontalHeader()->setSectionsMovable(true);
    this->ui->AssociationView->verticalHeader()->hide();
    this->ui->AssociationView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->AssociationView->setSelectionMode(QAbstractItemView::SingleSelection);

}

