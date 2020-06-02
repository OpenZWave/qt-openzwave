#include <QtDebug>
#ifndef _WIN32
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "qtozwdaemon.h"

Q_DECLARE_LOGGING_CATEGORY(ozwdaemon);

#ifndef _WIN32
int qtozwdaemon::sigtermFd[2] = {0, 0};
#endif

qtozwdaemon::qtozwdaemon(QObject *parent) : QObject(parent)
{

#ifndef _WIN32    
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
       qCCritical(ozwdaemon) << "Couldn't create TERM socketpair";
    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
#endif    
    
    
    QRegularExpression re("^(0[xX][a-fA-F0-9]{2}[ ,]*){16}$");
    
    QString NetworkKeyTest = qgetenv("OZW_NETWORK_KEY");
    QString NetworkKey;
    QRegularExpressionMatch match = re.match(NetworkKeyTest);
    if (match.hasMatch()) {
        NetworkKey = NetworkKeyTest;
        qCInfo(ozwdaemon) << "Network Key Specified in Enviroment is Valid";
    } else {
        qCWarning(ozwdaemon) << "Network Key Specified in Enviroment is Invalid";
    }
    
    QFile nwk_file("/run/secrets/OZW_Network_Key");
    if (nwk_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        NetworkKeyTest = nwk_file.readLine().trimmed();
        nwk_file.close();
        match = re.match(NetworkKeyTest);
        if (match.hasMatch()) {
            NetworkKey = NetworkKeyTest;
            qCInfo(ozwdaemon) << "Network Key From File is Valid - Using File";
        } else {
            if (NetworkKey.isEmpty()) {
                qCWarning(ozwdaemon) << "Network Key From File in Invalid - No Valid Network Key Found in Enviroment or File";
            }
        }
    } else {
        qCInfo(ozwdaemon) << "Didn't Find Network Key File. Skipping";
    }
    if (!NetworkKey.isEmpty()) {
        qCInfo(ozwdaemon) << "We Have what appears to be a valid Network Key - Passing to OZW";
    }

    QString AuthKey = qgetenv("OZW_AUTH_KEY");
    
    QFile ak_file("/run/secrets/OZW_Auth_Key");
    if (ak_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        AuthKey = ak_file.readLine().trimmed();
        ak_file.close();
    } else {
        qCInfo(ozwdaemon) << "Didn't Find Auth Key File. Skipping";
    }
    if (!AuthKey.isEmpty()) {
        qCInfo(ozwdaemon) << "Using Remote Authentication Key";
    }

    this->m_openzwave = new QTOpenZwave(this);
    this->m_qtozwmanager = this->m_openzwave->GetManager();
    QObject::connect(this->m_qtozwmanager, &QTOZWManager::readyChanged, this, &qtozwdaemon::QTOZW_Ready);
    this->m_qtozwmanager->initilizeSource(true);
    if (!NetworkKey.isEmpty()) {
        this->m_qtozwmanager->getOptions()->setNetworkKey(NetworkKey);
    }
    if (!AuthKey.isEmpty()) { 
        this->m_qtozwmanager->setClientAuth(AuthKey);
    }
}

void qtozwdaemon::QTOZW_Ready(bool ready) {
    if (ready) qCDebug(ozwdaemon) << "Ready";
}
void qtozwdaemon::startOZW() {
    if (getSerialPort().size() == 0) {
        qCWarning(ozwdaemon) << "Serial Port is not Set";
        return;
    }
    this->m_qtozwmanager->open(this->getSerialPort());
}

QTOZWManager *qtozwdaemon::getManager() {
    return this->m_qtozwmanager;
}

QTOpenZwave *qtozwdaemon::getQTOpenZWave() {
    return this->m_openzwave;
}

void qtozwdaemon::termSignalHandler(int unused) {
#ifndef _WIN32
    Q_UNUSED(unused);
        char a = 1;
    ::write(sigtermFd[0], &a, sizeof(a));
#endif
}

void qtozwdaemon::handleSigTerm()
{
#ifndef _WIN32
    snTerm->setEnabled(false);
    char tmp;
    ::read(sigtermFd[1], &tmp, sizeof(tmp));
    qCInfo(ozwdaemon) << "Recieved SIGTERM: Shutting down ozwdaemon";
    this->m_qtozwmanager->close();
    QCoreApplication::exit(0);
    snTerm->setEnabled(true);
#endif
}

void qtozwdaemon::aboutToQuit() {
    this->m_qtozwmanager->close();
}