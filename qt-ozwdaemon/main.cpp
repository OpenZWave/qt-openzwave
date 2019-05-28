#include <QCoreApplication>
#include <QLoggingCategory>
#include <QCommandLineParser>
#include "qtozwdaemon.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("qt-ozwdaemon");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationName("OpenZWave");
    QCoreApplication::setOrganizationDomain("openzwave.com");

    QCommandLineParser parser;
    parser.setApplicationDescription("QT OpenZWave Remote Daemon");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption serialPort(QStringList() << "s" << "serial-port",
        "Serial Port of USB Stick",
        "serialPort"
    );

    parser.addOption(serialPort);

    QCommandLineOption configDir(QStringList() << "c" << "config-dir",
        "Directory containing the OZW Config Files",
        "configDir"
    );

    parser.addOption(configDir);

    QCommandLineOption userDir(QStringList() << "u" << "user-dir",
        "Directory for the OZW User Files",
        "userDir"
    );

    parser.addOption(userDir);

    parser.process(a);
    if (!parser.isSet(serialPort)) {
        fputs(qPrintable("Serial Port is Required\n"), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        exit(-1);
    }


#if 1
    QLoggingCategory::setFilterRules("qt.remoteobjects.debug=true\n"
                                     "qt.remoteobjects.warning=true\n"
                                     "qt.remoteobjects.models.debug=true\n"
                                     "qt.remoteobjects.models.debug=true\n"
                                     "qt.remoteobjects.io.debug=true\n"
                                     "default.debug=true");
#else
    QLoggingCategory::setFilterRules("default.debug=true");
#endif
    qtozwdaemon daemon;
    daemon.setSerialPort(parser.value(serialPort));
    daemon.startOZW();
    return a.exec();
}
