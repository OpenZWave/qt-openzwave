#include <QDirIterator>
#include <QDebug>
#include "qt-openzwave/qt-openzwavedatabase.h"

bool copyConfigDatabase(QDir configpath)
{
    QDirIterator it(":/config/", QStringList() << "*.xml" << "*.png" << "*.xsd", QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        QString dirname = fi.dir().path().remove(0, 2);
        configpath.mkpath(dirname);
        if (!QFileInfo::exists(configpath.path()+fi.filePath().remove(0,1))) {
            qDebug() << "Copying " << fi.filePath() << " to " << configpath.filePath(fi.filePath().remove(0, 2));
            QFile::copy(fi.filePath(), configpath.filePath(fi.filePath().remove(0, 2)));
        } else {
            qDebug() << "Skipping " << fi.filePath() << " as it exists";
        }
    }
    return true;
}
