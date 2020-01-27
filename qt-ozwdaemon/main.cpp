
#ifdef BP_LINUX
#include "client/linux/handler/exception_handler.h"
#include "common/linux/http_upload.h"
#define HAVE_BP
#endif

#include <signal.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QCommandLineParser>
#ifndef _WIN32
#include <qt-openzwave/qt-openzwavedatabase.h>
#endif
#include "qtozwdaemon.h"
#ifdef HAVE_MQTT
#include "mqttpublisher.h"
#endif

#if defined(_Q_OS_LINUX)
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <cxxabi.h>

void backtrace(int sig = 0)
{
  Q_UNUSED(sig);
  unw_cursor_t cursor;
  unw_context_t context;

  qWarning() << "=============================";
  qWarning() << "CRASH!!! - Dumping Backtrace:";
  qWarning() << "=============================";

  unw_getcontext(&context);
  unw_init_local(&cursor, &context);

  int n=0;
  while ( unw_step(&cursor) ) {
    unw_word_t ip, sp, off;

    unw_get_reg(&cursor, UNW_REG_IP, &ip);
    unw_get_reg(&cursor, UNW_REG_SP, &sp);

    char symbol[256] = {"<unknown>"};
    char *name = symbol;

    if ( !unw_get_proc_name(&cursor, symbol, sizeof(symbol), &off) ) {
      int status;
      if ( (name = abi::__cxa_demangle(symbol, NULL, NULL, &status)) == 0 )
        name = symbol;
    }
    char message[1024];
    snprintf(message, sizeof(message), "#%-2d 0x%016" PRIxPTR " sp=0x%016" PRIxPTR " %s + 0x%" PRIxPTR,
        ++n,
        static_cast<uintptr_t>(ip),
        static_cast<uintptr_t>(sp),
        name,
        static_cast<uintptr_t>(off));
    qWarning() << message;
    if ( name != symbol )
      free(name);
  }
#ifndef HAVE_BP
    qWarning("Exiting....");
    exit(-1);
#endif
}

void crash() { volatile int* a = (int*)(NULL); *a = 1; }

#ifdef HAVE_BP
static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
void* context, bool succeeded) {
    backtrace();
    qWarning() << "dumpCallback Succeeded: " << succeeded << " at " << descriptor.path();
    if (succeeded == true) {
        std::map<string, string> parameters;
        std::map<string, string> files;
        std::string proxy_host;
        std::string proxy_userpasswd;
        std::string url("https://sentry.io/api/1868130/minidump/?sentry_key=e086ba93030843199aab391947d205da");

        // Add any attributes to the parameters map.
        // Note that several attributes are automatically extracted.
        parameters["product_name"] = QCoreApplication::applicationName().toStdString();
        parameters["release"] =  QCoreApplication::applicationVersion().toStdString();
        qtozwdaemon *daemon = static_cast<qtozwdaemon *>(context);
        parameters["OpenZWave_Version"] = daemon->getManager()->getVersionAsString().toStdString();
        parameters["QTOpenZWave_Version"] = daemon->getQTOpenZWave()->getVersion().toStdString();
        parameters["QT_Version"] = qVersion();

        files["upload_file_minidump"] = descriptor.path();

        std::string response, error;
        bool success = google_breakpad::HTTPUpload::SendRequest(url, parameters, files, proxy_host, proxy_userpasswd, "", &response, NULL, &error);
        if (success)
            qWarning() << "Uploaded Crash minidump With ID: " <<  response.c_str();
        else
            qWarning() << "Failed to Upload Crash MiniDump in " << descriptor.path();
    }

    return succeeded;
}
#endif

#endif /* Q_OS_LINUX */
int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("ozwdaemon");
    QCoreApplication::setApplicationVersion("0.1");
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

#ifdef HAVE_MQTT
    QCommandLineOption MQTTServer(QStringList() << "mqtt-server",
        "MQTT Server Hostname/IP Address",
        "IP/Hostname"
    );

    parser.addOption(MQTTServer);

    QCommandLineOption MQTTPort(QStringList() << "mqtt-port",
        "MQTT Server Port",
        "Port"
    );

    parser.addOption(MQTTPort);

    QCommandLineOption MQTTInstance(QStringList() << "mqtt-instance",
        "OpenZWave Instance Number - Defaults to 1",
        "Number"
    );

    parser.addOption(MQTTInstance);

    QCommandLineOption MQTTTLS(QStringList() << "mqtt-tls",
        "Enable TLS Encryption to MQTT Server"
    );

    parser.addOption(MQTTTLS);

#endif

    QCommandLineOption StopOnFailure(QStringList() << "stop-on-failure",
        "Exit on Driver Failure"
    );
    parser.addOption(StopOnFailure);


    parser.process(a);
    if (!parser.isSet(serialPort)) {
        fputs(qPrintable("Serial Port is Required\n"), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        exit(-1);
    }

#if 0
    QLoggingCategory::setFilterRules("qt.remoteobjects.debug=true\n"
                                     "qt.remoteobjects.warning=true\n"
                                     "qt.remoteobjects.models.debug=true\n"
                                     "qt.remoteobjects.models.debug=true\n"
                                     "qt.remoteobjects.io.debug=true\n"
                                     "default.debug=true");
#else
    QLoggingCategory::setFilterRules("*.debug=true");
#endif

    QStringList PossibleDBPaths;
    if (parser.isSet(configDir))
        PossibleDBPaths << parser.value(configDir);
    PossibleDBPaths << "./config/";
    PossibleDBPaths << QDir::toNativeSeparators("../../../config/");
    //PossibleDBPaths << settings.value("openzwave/ConfigPath", QDir::toNativeSeparators("../../../config/")).toString().append("/");
    PossibleDBPaths << QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);

    QString path, dbPath, userPath;
    foreach(path, PossibleDBPaths) {
        qDebug() << "Checking " << QFileInfo(QDir::toNativeSeparators(path+"/config/manufacturer_specific.xml")).absoluteFilePath() << " for manufacturer_specific.xml";
        if (QFileInfo(QDir::toNativeSeparators(path+"/config/manufacturer_specific.xml")).exists()) {
            dbPath = QFileInfo(QDir::toNativeSeparators(path+"/config/manufacturer_specific.xml")).absoluteFilePath();
            break;
        }
        qDebug() << "Checking " << QFileInfo(QDir::toNativeSeparators(path+"../config/manufacturer_specific.xml")).absoluteFilePath() << " for manufacturer_specific.xml";
        if (QFile(QDir::toNativeSeparators(path+"/../config/manufacturer_specific.xml")).exists()) {
            dbPath = QFileInfo(QDir::toNativeSeparators(path+"/../config/manufacturer_specific.xml")).absoluteFilePath();
            break;
        }
    }
    PossibleDBPaths.clear();
    if (parser.isSet(userDir))
        PossibleDBPaths << parser.value(userDir);
    PossibleDBPaths << "./config/";
    PossibleDBPaths <<  QDir::toNativeSeparators("../../../config/");
    PossibleDBPaths << QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    PossibleDBPaths << "/usr/share/qt5/";
    
    foreach(path, PossibleDBPaths) {
        qDebug() << "Checking " << QFileInfo(QDir::toNativeSeparators(path+"/config/Options.xml")).absoluteFilePath() << " for options.xml";
        if (QFileInfo(QDir::toNativeSeparators(path+"/config/options.xml")).exists()) {
            userPath = QFileInfo(QDir::toNativeSeparators(path+"/config/options.xml")).absoluteFilePath();
            break;
        }
        qDebug() << "Checking " << QFileInfo(QDir::toNativeSeparators(path+"/../config/options.xml")).absoluteFilePath() << " for options.xml";
        if (QFile(QDir::toNativeSeparators(path+"/../config/options.xml")).exists()) {
            userPath = QFileInfo(QDir::toNativeSeparators(path+"/../config/options.xml")).absoluteFilePath();
            break;
        }
    }

#if defined(Q_OS_LINUX) 
    if (dbPath.isEmpty()) {
        if (initConfigDatabase(PossibleDBPaths)) {
            copyConfigDatabase(QFileInfo("./").absoluteFilePath().append("/"));
            dbPath = "./config/";
            userPath = "./config/";
        } else {
            qWarning() << "Cant find qt-openzwavedatabase.rcc";
        }
    }
#endif
    qDebug() << "DBPath: " << dbPath;
    qDebug() << "userPath: " << userPath;

    QSettings settings(userPath.append("/ozwdaemon.ini"), QSettings::IniFormat);

#ifdef HAVE_MQTT
    if (parser.isSet(MQTTServer)) {
        settings.setValue("MQTTServer", parser.value(MQTTServer));
    }
    if (parser.isSet(MQTTPort)) {
        settings.setValue("MQTTPort", parser.value(MQTTPort).toInt());
    }
    if (parser.isSet(MQTTInstance)) {
        settings.setValue("Instance", parser.value(MQTTInstance).toInt());
    }
    if (parser.isSet(MQTTTLS)) {
        qDebug() << "mqtt tls set";
        settings.setValue("MQTTTLS", true);
    } else {
        settings.setValue("MQTTTLS", false);
    }
#endif

    if (parser.isSet(StopOnFailure)) {
        settings.setValue("StopOnFailure", true);
    }

    QTOZWOptions options(QTOZWOptions::Local);
    options.setUserPath(userPath);
    options.setConfigPath(dbPath);



    qtozwdaemon daemon;
#ifdef HAVE_MQTT
    mqttpublisher mqttpublisher(&settings);
    mqttpublisher.setOZWDaemon(&daemon);
#endif

#if defined(Q_OS_LINUX)
#ifdef HAVE_BP
    QString bppath =  QString::fromLocal8Bit(qgetenv("BP_DB_PATH"));
    if (bppath.isEmpty())
        bppath = QStandardPaths::standardLocations(QStandardPaths::TempLocation).at(0);
    qInfo() << "Using BreakPad - Crash Directory: " << bppath;
    /* ensure path exists */
    QDir dir;
    if (!dir.exists(bppath)) {
            dir.mkpath(bppath);
    }
    google_breakpad::MinidumpDescriptor descriptor(bppath.toStdString());
    google_breakpad::ExceptionHandler eh(descriptor, NULL, dumpCallback, static_cast<void *>(&daemon), true, -1);
#else
    signal(SIGSEGV, backtrace);
    signal(SIGABRT, backtrace);
#endif
#endif


    daemon.setSerialPort(parser.value(serialPort));
    daemon.startOZW();
//    assert(0);
//    crash();
    return a.exec();
}
