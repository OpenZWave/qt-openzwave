#ifndef QTOPENZWAVEDATABASE_H
#define QTOPENZWAVEDATABASE_H
#include <QDir>
#include <QLoggingCategory>

#if defined(QTOZWDATABASE_SHARED_LIBRARY)
#  define QTOZWDATABASE_EXPORT Q_DECL_EXPORT
#else
#  define QTOZWDATABASE_EXPORT Q_DECL_IMPORT
#endif

Q_DECLARE_LOGGING_CATEGORY(qtozwdatabase);

QTOZWDATABASE_EXPORT bool initConfigDatabase(QStringList paths);
QTOZWDATABASE_EXPORT bool copyConfigDatabase(QDir configpath);

#endif // QTOPENZWAVEDATABASE_H
