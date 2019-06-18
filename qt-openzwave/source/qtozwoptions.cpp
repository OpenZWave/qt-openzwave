#include <QAbstractItemModelReplica>

#include "qt-openzwave/qtozwoptions.h"
#include "qtozwoptions_p.h"
#include "qtozw_logging.h"

#define CONNECT_DPTR(x)     if (this->m_connectionType == connectionType::Local) { \
        QObject::connect(this->d_ptr_internal, &QTOZWOptions_Internal::x, this, &QTOZWOptions::x);\
    } else { \
        QObject::connect(this->d_ptr_replica, &QTOZWOptionsReplica::x, this, &QTOZWOptions::x); \
    };

#define CONNECT_DPTR1(x, y)     if (this->m_connectionType == connectionType::Local) { \
        QObject::connect(this->d_ptr_internal, &QTOZWOptions_Internal::x, this, &QTOZWOptions::y);\
    } else { \
        QObject::connect(this->d_ptr_replica, &QTOZWOptionsReplica::x, this, &QTOZWOptions::y); \
    };

#define CALL_DPTR(x) if (this->m_connectionType == QTOZWOptions::connectionType::Local) this->d_ptr_internal->x; else this->d_ptr_replica->x;
#define CALL_DPTR_PROP(x) if (this->m_connectionType == QTOZWOptions::connectionType::Local) return this->d_ptr_internal->x; else return this->d_ptr_replica->x;
#define CALL_DPTR_PROP_SET(x) if (this->m_connectionType == QTOZWOptions::connectionType::Local) { \
        this->d_ptr_internal->setProperty(#x, x); \
    } else { \
        this->d_ptr_replica->setProperty(#x, x); \
    }
#define CALL_DPTR_PROP_SET_TYPE(x, y) if (this->m_connectionType == QTOZWOptions::connectionType::Local) { \
        this->d_ptr_internal->setProperty(#x, QVariant::fromValue<y>(x)); \
    } else { \
        this->d_ptr_replica->setProperty(#x, QVariant::fromValue<y>(x)); \
    }
#define CALL_DPTR_METHOD_RTN(x, y) if (this->m_connectionType == QTOZWOptions::connectionType::Local) \
    return this->d_ptr_internal->x; \
    else { \
    QRemoteObjectPendingReply<y> res = this->d_ptr_replica->x; \
    res.waitForFinished(); \
    return res.returnValue(); \
    }




QTOZWOptions::QTOZWOptions(connectionType type, QObject *parent)
    : QObject(parent),
      m_connectionType(type)
{
    if (type == QTOZWOptions::connectionType::Local) {
        this->m_connectionType = connectionType::Local;
        this->d_ptr_internal = new QTOZWOptions_Internal(this);
        connectSignals();
    } else if (type == connectionType::Remote) {
        qDebug() << "Nothing to do till we connect";
    }
}

bool QTOZWOptions::initilizeBase() {
    return true;
}

bool QTOZWOptions::initilizeSource(QRemoteObjectHost *m_sourceNode) {
    initilizeBase();
    if (m_sourceNode) {
        m_sourceNode->enableRemoting<QTOZWOptionsSourceAPI>(this->d_ptr_internal);
    }
    return true;
}

bool QTOZWOptions::initilizeReplica(QRemoteObjectNode *m_replicaNode) {
    initilizeBase();
    this->m_connectionType = connectionType::Remote;
    this->d_ptr_replica = m_replicaNode->acquire<QTOZWOptionsReplica>("QTOZWOptions");
    QObject::connect(this->d_ptr_replica, &QTOZWOptionsReplica::stateChanged, this, &QTOZWOptions::onOptionStateChange);
    connectSignals();
    return true;
}

void QTOZWOptions::connectSignals() {
    CONNECT_DPTR(ConfigPathChanged);
    CONNECT_DPTR(UserPathChanged);
    CONNECT_DPTR(LoggingChanged);
    CONNECT_DPTR(LogFileNameChanged);
    CONNECT_DPTR(AppendLogFileChanged);
    CONNECT_DPTR(ConsoleOutputChanged);
    CONNECT_DPTR(SaveLogLevelChanged);
    CONNECT_DPTR(QueueLogLevelChanged);
    CONNECT_DPTR(DumpTriggerLevelChanged);
    CONNECT_DPTR(AssociateChanged);
    CONNECT_DPTR(ExcludeChanged);
    CONNECT_DPTR(IncludeChanged);
    CONNECT_DPTR(NotifyTransactionsChanged);
    CONNECT_DPTR(InterfaceChanged);
    CONNECT_DPTR(SaveConfigurationChanged);
    CONNECT_DPTR(DriverMaxAttemptsChanged);
    CONNECT_DPTR(PollIntervalChanged);
    CONNECT_DPTR(IntervalBetweenPollsChanged);
    CONNECT_DPTR(SuppressValueRefreshChanged);
    CONNECT_DPTR(PerformReturnRoutesChanged);
    CONNECT_DPTR(NetworkKeyChanged);
    CONNECT_DPTR(RefreshAllUserCodesChanged);
    CONNECT_DPTR(RetryTimeoutChanged);
    CONNECT_DPTR(EnableSISChanged);
    CONNECT_DPTR(AssumeAwakeChanged);
    CONNECT_DPTR(NotifyOnDriverUnloadChanged);
    CONNECT_DPTR(SecurityStrategyChanged);
    CONNECT_DPTR(CustomSecuredCCChanged);
    CONNECT_DPTR(EnforceSecureReceptionChanged);
    CONNECT_DPTR(AutoUpdateConfigFileChanged);
    CONNECT_DPTR(ReloadAfterUpdateChanged);
    CONNECT_DPTR(LanguageChanged);
    CONNECT_DPTR(IncludeInstanceLabelsChanged);
//    CONNECT_DPTR(error);
}

QString QTOZWOptions::ConfigPath() const {
    CALL_DPTR_PROP(ConfigPath());
}
QString QTOZWOptions::UserPath() const {
    CALL_DPTR_PROP(UserPath());
}
bool QTOZWOptions::Logging() const {
    CALL_DPTR_PROP(Logging());
}
QString QTOZWOptions::LogFileName() const {
    CALL_DPTR_PROP(LogFileName());
}
bool QTOZWOptions::AppendLogFile() const {
    CALL_DPTR_PROP(AppendLogFile());
}
bool QTOZWOptions::ConsoleOutput() const {
    CALL_DPTR_PROP(ConsoleOutput());
}
OptionList QTOZWOptions::SaveLogLevel() const {
    CALL_DPTR_PROP(SaveLogLevel());
}
OptionList QTOZWOptions::QueueLogLevel() const {
    CALL_DPTR_PROP(QueueLogLevel());
}
OptionList QTOZWOptions::DumpTriggerLevel() const {
    CALL_DPTR_PROP(DumpTriggerLevel());
}
bool QTOZWOptions::Associate() const {
    CALL_DPTR_PROP(Associate());
}
QString QTOZWOptions::Exclude() const {
    CALL_DPTR_PROP(Exclude());
}
QString QTOZWOptions::Include() const {
    CALL_DPTR_PROP(Include());
}
bool QTOZWOptions::NotifyTransactions() const {
    CALL_DPTR_PROP(NotifyTransactions());
}
QString QTOZWOptions::Interface() const {
    CALL_DPTR_PROP(Interface());
}
bool QTOZWOptions::SaveConfiguration() const {
    CALL_DPTR_PROP(SaveConfiguration());
}
qint32 QTOZWOptions::DriverMaxAttempts() const {
    CALL_DPTR_PROP(DriverMaxAttempts());
}
qint32 QTOZWOptions::PollInterval() const {
    CALL_DPTR_PROP(PollInterval());
}
bool QTOZWOptions::SuppressValueRefresh() const {
    CALL_DPTR_PROP(SuppressValueRefresh());
}
bool QTOZWOptions::IntervalBetweenPolls() const {
    CALL_DPTR_PROP(IntervalBetweenPolls());
}
bool QTOZWOptions::PerformReturnRoutes() const {
    CALL_DPTR_PROP(PerformReturnRoutes());
}
QString QTOZWOptions::NetworkKey() const {
    CALL_DPTR_PROP(NetworkKey());
}
bool QTOZWOptions::RefreshAllUserCodes() const {
    CALL_DPTR_PROP(RefreshAllUserCodes());
}
qint32 QTOZWOptions::RetryTimeout() const {
    CALL_DPTR_PROP(RetryTimeout());
}
bool QTOZWOptions::EnableSIS() const {
    CALL_DPTR_PROP(EnableSIS());
}
bool QTOZWOptions::AssumeAwake() const {
    CALL_DPTR_PROP(AssumeAwake());
}
bool QTOZWOptions::NotifyOnDriverUnload() const {
    CALL_DPTR_PROP(NotifyTransactions());
}
QString QTOZWOptions::SecurityStrategy() const {
    CALL_DPTR_PROP(SecurityStrategy());
}
QString QTOZWOptions::CustomSecuredCC() const {
    CALL_DPTR_PROP(CustomSecuredCC());
}
bool QTOZWOptions::EnforceSecureReception() const {
    CALL_DPTR_PROP(EnforceSecureReception());
}
bool QTOZWOptions::AutoUpdateConfigFile() const {
    CALL_DPTR_PROP(AutoUpdateConfigFile());
}
QString QTOZWOptions::ReloadAfterUpdate() const {
    CALL_DPTR_PROP(ReloadAfterUpdate());
}
QString QTOZWOptions::Language() const {
    CALL_DPTR_PROP(Language());
}
bool QTOZWOptions::IncludeInstanceLabels() const {
    CALL_DPTR_PROP(IncludeInstanceLabels());
}

void QTOZWOptions::setConfigPath(QString ConfigPath) {
    CALL_DPTR_PROP_SET(ConfigPath);
}
void QTOZWOptions::setUserPath(QString UserPath) {
    CALL_DPTR_PROP_SET(UserPath);
}
void QTOZWOptions::setLogging(bool Logging) {
    CALL_DPTR_PROP_SET(Logging);
}
void QTOZWOptions::setLogFileName(QString LogFileName) {
    CALL_DPTR_PROP_SET(LogFileName);
}
void QTOZWOptions::setAppendLogFile(bool AppendLogFile) {
    CALL_DPTR_PROP_SET(AppendLogFile);
}
void QTOZWOptions::setConsoleOutput(bool ConsoleOutput) {
    CALL_DPTR_PROP_SET(ConsoleOutput);
}
void QTOZWOptions::setSaveLogLevel(OptionList SaveLogLevel) {
    CALL_DPTR_PROP_SET_TYPE(SaveLogLevel, OptionList);
}
void QTOZWOptions::setQueueLogLevel(OptionList QueueLogLevel) {
    CALL_DPTR_PROP_SET_TYPE(QueueLogLevel, OptionList);
}
void QTOZWOptions::setDumpTriggerLevel(OptionList DumpTriggerLevel) {
    CALL_DPTR_PROP_SET_TYPE(DumpTriggerLevel, OptionList);
}
void QTOZWOptions::setAssociate(bool Associate) {
    CALL_DPTR_PROP_SET(Associate);
}
void QTOZWOptions::setExclude(QString Exclude) {
    CALL_DPTR_PROP_SET(Exclude);
}
void QTOZWOptions::setInclude(QString Include) {
    CALL_DPTR_PROP_SET(Include);
}
void QTOZWOptions::setNotifyTransactions(bool NotifyTransactions) {
    CALL_DPTR_PROP_SET(NotifyTransactions);
}
void QTOZWOptions::setInterface(QString Interface) {
    CALL_DPTR_PROP_SET(Interface);
}
void QTOZWOptions::setSaveConfiguration(bool SaveConfiguration) {
    CALL_DPTR_PROP_SET(SaveConfiguration);
}
void QTOZWOptions::setDriverMaxAttempts(qint32 DriverMaxAttempts) {
    CALL_DPTR_PROP_SET(DriverMaxAttempts);
}
void QTOZWOptions::setPollInterval(qint32 PollInterval) {
    CALL_DPTR_PROP_SET(PollInterval);
}
void QTOZWOptions::setIntervalBetweenPolls(bool IntervalBetweenPolls) {
    CALL_DPTR_PROP_SET(IntervalBetweenPolls);
}
void QTOZWOptions::setSuppressValueRefresh(bool SuppressValueRefresh) {
    CALL_DPTR_PROP_SET(SuppressValueRefresh);
}
void QTOZWOptions::setPerformReturnRoutes(bool PerformReturnRoutes) {
    CALL_DPTR_PROP_SET(PerformReturnRoutes);
}
void QTOZWOptions::setNetworkKey(QString NetworkKey) {
    CALL_DPTR_PROP_SET(NetworkKey);
}
void QTOZWOptions::setRefreshAllUserCodes(bool RefreshAllUserCodes) {
    CALL_DPTR_PROP_SET(RefreshAllUserCodes);
}
void QTOZWOptions::setRetryTimeout(qint32 RetryTimeout) {
    CALL_DPTR_PROP_SET(RetryTimeout);
}
void QTOZWOptions::setEnableSIS(bool EnableSIS) {
    CALL_DPTR_PROP_SET(EnableSIS);
}
void QTOZWOptions::setAssumeAwake(bool AssumeAwake) {
    CALL_DPTR_PROP_SET(AssumeAwake);
}
void QTOZWOptions::setNotifyOnDriverUnload(bool NotifyOnDriverUnload) {
    CALL_DPTR_PROP_SET(NotifyOnDriverUnload);
}
void QTOZWOptions::setSecurityStrategy(QString SecurityStrategy) {
    CALL_DPTR_PROP_SET(SecurityStrategy);
}
void QTOZWOptions::setCustomSecuredCC(QString CustomSecuredCC) {
    CALL_DPTR_PROP_SET(CustomSecuredCC);
}
void QTOZWOptions::setEnforceSecureReception(bool EnforceSecureReception) {
    CALL_DPTR_PROP_SET(EnforceSecureReception);
}
void QTOZWOptions::setAutoUpdateConfigFile(bool AutoUpdateConfigFile) {
    CALL_DPTR_PROP_SET(AutoUpdateConfigFile);
}
void QTOZWOptions::setReloadAfterUpdate(QString ReloadAfterUpdate) {
    CALL_DPTR_PROP_SET(ReloadAfterUpdate);
}
void QTOZWOptions::setLanguage(QString Language) {
    CALL_DPTR_PROP_SET(Language);
}
void QTOZWOptions::setIncludeInstanceLabels(bool IncludeInstanceLabels) {
    CALL_DPTR_PROP_SET(IncludeInstanceLabels);
}

bool QTOZWOptions::AddOptionBool(QString option, bool value) {
    CALL_DPTR_METHOD_RTN(AddOptionBool(option, value), bool);
}
bool QTOZWOptions::AddOptionInt(QString option, qint32 value) {
    CALL_DPTR_METHOD_RTN(AddOptionInt(option, value), bool)
}
bool QTOZWOptions::AddOptionString(QString option, QString value) {
    CALL_DPTR_METHOD_RTN(AddOptionString(option, value), bool)
}
bool QTOZWOptions::GetOptionAsBool(QString option) {
    CALL_DPTR_METHOD_RTN(GetOptionAsBool(option), bool);
}
qint32 QTOZWOptions::GetOptionAsInt(QString option) {
    CALL_DPTR_METHOD_RTN(GetOptionAsInt(option), qint32);
}
QString QTOZWOptions::GetOptionAsString(QString option) {
    CALL_DPTR_METHOD_RTN(GetOptionAsString(option), QString);
}

bool QTOZWOptions::isLocked() {
    CALL_DPTR_METHOD_RTN(isLocked(), bool);
}
