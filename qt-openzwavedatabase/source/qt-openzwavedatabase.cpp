#include <QDirIterator>
#include <QDebug>
#include <QResource>
#include <QStandardPaths>
#include "qt-openzwave/qt-openzwavedatabase.h"

Q_LOGGING_CATEGORY(qtozwdatabase, "ozw.database");

bool initConfigDatabase(QStringList path) {
    for (int i = 0; i < path.size(); ++i) {
        QString test = path.at(i);
        test.append("/qt-openzwavedatabase.rcc");
        qCDebug(qtozwdatabase) << "Testing " << test;
        if (QResource::registerResource(test)) {
            qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << test;
            return true;
        }
    }
    if (QResource::registerResource("qt-openzwavedatabase/qt-openzwavedatabase.rcc")) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at qt-openzwavedatabase/qt-openzwavedatabase.rcc";
        return true;
    }
    if (QResource::registerResource("./qt-openzwavedatabase.rcc")) {
        qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at ./qt-openzwavedatabase.rcc";
        return true;
    }

    for (int i = 0; i < QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).size(); ++i) {
        QString test = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).at(i);
        test.append("/OpenZWave/qt-openzwavedatabase.rcc");
        qCDebug(qtozwdatabase) << "Testing Standard Paths" << test;
        if (QResource::registerResource(test)) {
            qCInfo(qtozwdatabase) << "Found qt-openzwavedatabase.rcc at " << test;
            return true;
        }
    }

    return false; 

}


bool copyConfigDatabase(QDir configpath)
{
    QDirIterator it(":/config/", QStringList() << "*.xml" << "*.png" << "*.xsd", QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        QString dirname = fi.dir().path().remove(0, 2);
        configpath.mkpath(dirname);
        if (!QFileInfo::exists(configpath.path()+fi.filePath().remove(0,1))) {
            qCDebug(qtozwdatabase) << "Copying " << fi.filePath() << " to " << configpath.filePath(fi.filePath().remove(0, 2));
            QFile::copy(fi.filePath(), configpath.filePath(fi.filePath().remove(0, 2)));
        } else {
            qCDebug(qtozwdatabase) << "Skipping " << fi.filePath() << " as it exists";
        }
    }
    return true;
}
