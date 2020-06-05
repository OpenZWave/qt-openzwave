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
        qCDebug(qtozwdatabase) << "Testing " << test;
        if (QResource::registerResource(test)) {
            qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << test;
            rccpath = test;
            return true;
        }
    }
#if defined(Q_OS_OSX)
    if (QResource::registerResource(QCoreApplication::applicationDirPath() + "/../Resources/qt-openzwavedatabase.rcc")) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QCoreApplication::applicationDirPath() + "/../Resources/";
        rccpath = QCoreApplication::applicationDirPath() + "/../Resources/qt-openzwavedatabase.rcc";
        return true;
    }
#endif
    if (QResource::registerResource("qt-openzwavedatabase/qt-openzwavedatabase.rcc")) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at qt-openzwavedatabase/qt-openzwavedatabase.rcc";
        rccpath = "qt-openzwavedatabase/qt-openzwavedatabase.rcc";
        return true;
    }
    if (QResource::registerResource("./qt-openzwavedatabase.rcc")) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at ./qt-openzwavedatabase.rcc";
        rccpath = "./qt-openzwavedatabase.rcc";
        return true;
    }

    for (int i = 0; i < QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).size(); ++i) {
        QString test = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).at(i);
        test.append("/OpenZWave/qt-openzwavedatabase.rcc");
        qCDebug(qtozwdatabase) << "Testing Standard Paths" << test;
        if (QResource::registerResource(test)) {
            qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << test;
            rccpath = test;
            return true;
        } else if (QResource::registerResource(QCoreApplication::applicationDirPath() + test))
            qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << QCoreApplication::applicationDirPath() + test;
            rccpath = QCoreApplication::applicationDirPath() + test;
            return true;
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

