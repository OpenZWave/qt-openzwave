#include <QDirIterator>
#include <QDebug>
#include <QResource>
#include <QStandardPaths>
#include <QCoreApplication>
#include "qt-openzwave/qt-openzwavedatabase.h"

Q_LOGGING_CATEGORY(qtozwdatabase, "ozw.database");
QString rccpath;

bool initConfigDatabase(QStringList path) {
    rccpath = "";
    for (int i = 0; i < path.size(); ++i) {
        QString test = path.at(i);
        test.append("/qt-openzwavedatabase.rcc");
        qCDebug(qtozwdatabase) << "Testing " << QFileInfo(test).absoluteFilePath();
        if (QResource::registerResource(QFileInfo(test).absoluteFilePath())) {
            qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QFileInfo(test).absoluteFilePath();
            rccpath = test;
            return true;
        }
    }
#if defined(Q_OS_OSX)
    qCDebug(qtozwdatabase) << "Testng " << QFileInfo(QCoreApplication::applicationDirPath() + "/../Resources/qt-openzwavedatabase.rcc").absoluteFilePath();
    if (QResource::registerResource(QFileInfo(QCoreApplication::applicationDirPath() + "/../Resources/qt-openzwavedatabase.rcc").absoluteFilePath())) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QFileInfo(QCoreApplication::applicationDirPath() + "/../Resources/").absoluteFilePath();
        rccpath = QFileInfo(QCoreApplication::applicationDirPath() + "/../Resources/qt-openzwavedatabase.rcc").absoluteFilePath();
        return true;
    }
#endif
    qCDebug(qtozwdatabase) << "Testing " << QFileInfo("qt-openzwavedatabase.rcc").absoluteFilePath();
    if (QResource::registerResource(QFileInfo("qt-openzwavedatabase.rcc").absoluteFilePath())) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QFileInfo("qt-openzwavedatabase.rcc").absoluteFilePath();
        rccpath = QFileInfo("qt-openzwavedatabase.rcc").absoluteFilePath();
        return true;
    }
    qCDebug(qtozwdatabase) << "Testing " << QFileInfo("qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath();
    if (QResource::registerResource(QFileInfo("qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath())) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QFileInfo("qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath();
        rccpath = QFileInfo("qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath();
        return true;
    }
    qCDebug(qtozwdatabase) << "Testing " << QFileInfo("./qt-openzwavedatabase.rcc").absoluteFilePath();
    if (QResource::registerResource(QFileInfo("./qt-openzwavedatabase.rcc").absoluteFilePath())) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QFileInfo("./qt-openzwavedatabase.rcc").absoluteFilePath();
        rccpath = QFileInfo("./qt-openzwavedatabase.rcc").absoluteFilePath();
        return true;
    }

    qCDebug(qtozwdatabase) << "Testing " << QFileInfo("../qt-openzwave/qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath();
    if (QResource::registerResource(QFileInfo("../qt-openzwave/qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath())) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QFileInfo("../qt-openzwave/qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath();
        rccpath = QFileInfo("../qt-openzwave/qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath();
        return true;
    }

    qCDebug(qtozwdatabase) << "Testing " << QFileInfo("../../qt-openzwave/qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath();
    if (QResource::registerResource(QFileInfo("../../qt-openzwave/qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath())) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QFileInfo("../../qt-openzwave/qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath();
        rccpath = QFileInfo("../../qt-openzwave/qt-openzwavedatabase/qt-openzwavedatabase.rcc").absoluteFilePath();
        return true;
    }


    for (int i = 0; i < QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).size(); ++i) {
        QString test = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).at(i);
        test.append("/OpenZWave/qt-openzwavedatabase.rcc");
        qCDebug(qtozwdatabase) << "Testing Standard Paths" << QFileInfo(test).absoluteFilePath();
        if (QResource::registerResource(QFileInfo(test).absoluteFilePath())) {
            qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QFileInfo(test).absoluteFilePath();
            rccpath = QFileInfo(test).absoluteFilePath();
            return true;
#ifndef Q_OS_WIN
        } else {
            qCDebug(qtozwdatabase) << "Testing Standard Paths: "<< QFileInfo(QCoreApplication::applicationDirPath() + test).absoluteFilePath();
            if (QResource::registerResource(QFileInfo(QCoreApplication::applicationDirPath() + test).absoluteFilePath())) {
                qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QFileInfo(QCoreApplication::applicationDirPath() + test).absoluteFilePath();
                rccpath = QFileInfo(QCoreApplication::applicationDirPath() + test).absoluteFilePath();
                return true;
            }
#endif
        }
    }

    return false; 

}

bool finiConfigDatabase() {
    if (!rccpath.isEmpty())
       return QResource::unregisterResource(rccpath);
    return false;
}



bool copyConfigDatabase(QDir configpath)
{
    QDirIterator it(":/config/", QStringList() << "*.xml" << "*.png" << "*.xsd", QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        if (fi.filePath() == ":/config/options.xml") continue;
        QString dirname = fi.dir().path().remove(":/config/");
        configpath.mkpath(dirname);
        if (!QFileInfo::exists(configpath.path()+fi.filePath().remove(0,1))) {
            qCDebug(qtozwdatabase) << "copyConfigDatabase: Copying " << fi.filePath().remove(":/config/") << " to " << configpath.filePath(fi.filePath().remove(":/config/"));
            QFile::copy(fi.filePath(), configpath.filePath(fi.filePath().remove(":/config/")));
        } else {
            qCDebug(qtozwdatabase) << "copyConfigDatabase: Skipping " << fi.filePath().remove(":/config/") << " as it exists";
        }
    }
    return true;
}

bool copyUserDatabase(QDir configpath)
{
    QDirIterator it(":/config/", QStringList() << "options.xml", QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        configpath.mkpath(".");
        if (!QFileInfo::exists(configpath.path()+fi.filePath().remove(0,1))) {
            QDir target = configpath.filePath(fi.filePath().remove(":/config/"));    
            qCDebug(qtozwdatabase) << "copyUserDatabase: Copying " << fi.filePath().remove(":/config/") << " to " << target.path();
            QFile::copy(fi.filePath(), target.path());
        } else {
            qCDebug(qtozwdatabase) << "copyUserDatabase: Skipping " << fi.filePath().remove(":/config/") << " as it exists";
        }
    }
    return true;
}

