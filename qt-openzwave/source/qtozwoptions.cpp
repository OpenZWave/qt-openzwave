//-----------------------------------------------------------------------------
//
//	qtozwoptions.cpp
//
//	OpenZWave Options Class Wrapper
//
//	Copyright (c) 2019 Justin Hammond <Justin@dynam.ac>
//
//	SOFTWARE NOTICE AND LICENSE
//
//	This file is part of QT-OpenZWave.
//
//	OpenZWave is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Lesser General Public License as published
//	by the Free Software Foundation, either version 3 of the License,
//	or (at your option) any later version.
//
//	OpenZWave is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public License
//	along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------

#include <QAbstractItemModelReplica>

#include "qt-openzwave/qtozwoptions.h"
#include "qtozwoptions_p.h"
#include "qtozw_logging.h"

#define REP_INTERNAL_CLASS QTOZWOptions_Internal
#define REP_REPLICA_CLASS QTOZWOptionsReplica
#define REP_PUBLIC_CLASS QTOZWOptions


QTOZWOptions::QTOZWOptions(ConnectionType::Type type, QString localConfigPath, QString localUserPath, QObject *parent)
    : QTOZWReplicaBase(type, parent),
    d_ptr_internal(nullptr),
    d_ptr_replica(nullptr),
    m_localConfigPath(localConfigPath),
    m_localUserPath(localUserPath)
{ 
#ifndef Q_OS_WASM    
    if ((this->getConnectionType() == ConnectionType::Type::Local) || (this->getConnectionType() == ConnectionType::Invalid)) {
        if (this->getConnectionType() == ConnectionType::Type::Invalid)
            this->setConnectionType(ConnectionType::Type::Local);
        this->d_ptr_internal = new QTOZWOptions_Internal(m_localConfigPath, m_localUserPath, this);
        connectSignals();
    } else if (this->getConnectionType() == ConnectionType::Type::Remote) {
        qCDebug(options) << "Nothing to do till we connect";
    }
#endif
}
QTOZWOptions::~QTOZWOptions() {
}



bool QTOZWOptions::initilizeBase() {
    return true;
}
#ifndef Q_OS_WASM
bool QTOZWOptions::initilizeSource(QRemoteObjectHost *m_sourceNode) {
    this->setConnectionType(ConnectionType::Type::Local);

    initilizeBase();

    if (m_sourceNode) {
        m_sourceNode->enableRemoting<QTOZWOptionsSourceAPI>(this->d_ptr_internal);
    }

    return true;
}
#endif

bool QTOZWOptions::initilizeReplica(QRemoteObjectNode *m_replicaNode) {
    this->setConnectionType(ConnectionType::Type::Remote);

    initilizeBase();

    this->d_ptr_replica = m_replicaNode->acquire<QTOZWOptionsReplica>("QTOZWOptions");
    QObject::connect(this->d_ptr_replica, &QTOZWOptionsReplica::stateChanged, this, &QTOZWOptions::onOptionStateChange);

    disconnectSignals();

    connectSignals();

    return true;
}

void QTOZWOptions::disconnectSignals() {
    DISCONNECT_DPTR(ConfigPathChanged);
    DISCONNECT_DPTR(UserPathChanged);
    DISCONNECT_DPTR(LoggingChanged);
    DISCONNECT_DPTR(LogFileNameChanged);
    DISCONNECT_DPTR(AppendLogFileChanged);
    DISCONNECT_DPTR(ConsoleOutputChanged);
    DISCONNECT_DPTR(SaveLogLevelChanged);
    DISCONNECT_DPTR(QueueLogLevelChanged);
    DISCONNECT_DPTR(DumpTriggerLevelChanged);
    DISCONNECT_DPTR(AssociateChanged);
    DISCONNECT_DPTR(ExcludeChanged);
    DISCONNECT_DPTR(IncludeChanged);
    DISCONNECT_DPTR(NotifyTransactionsChanged);
    DISCONNECT_DPTR(InterfaceChanged);
    DISCONNECT_DPTR(SaveConfigurationChanged);
    DISCONNECT_DPTR(DriverMaxAttemptsChanged);
    DISCONNECT_DPTR(PollIntervalChanged);
    DISCONNECT_DPTR(IntervalBetweenPollsChanged);
    DISCONNECT_DPTR(SuppressValueRefreshChanged);
    DISCONNECT_DPTR(PerformReturnRoutesChanged);
    DISCONNECT_DPTR(NetworkKeyChanged);
    DISCONNECT_DPTR(RefreshAllUserCodesChanged);
    DISCONNECT_DPTR(RetryTimeoutChanged);
    DISCONNECT_DPTR(EnableSISChanged);
    DISCONNECT_DPTR(AssumeAwakeChanged);
    DISCONNECT_DPTR(NotifyOnDriverUnloadChanged);
    DISCONNECT_DPTR(SecurityStrategyChanged);
    DISCONNECT_DPTR(CustomSecuredCCChanged);
    DISCONNECT_DPTR(EnforceSecureReceptionChanged);
    DISCONNECT_DPTR(AutoUpdateConfigFileChanged);
    DISCONNECT_DPTR(ReloadAfterUpdateChanged);
    DISCONNECT_DPTR(LanguageChanged);
    DISCONNECT_DPTR(IncludeInstanceLabelsChanged);

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
    CALL_DPTR_PROP(ConfigPath(), QString());
}
QString QTOZWOptions::UserPath() const {
    CALL_DPTR_PROP(UserPath(), QString());
}
bool QTOZWOptions::Logging() const {
    CALL_DPTR_PROP(Logging(), false);
}
QString QTOZWOptions::LogFileName() const {
    CALL_DPTR_PROP(LogFileName(), QString());
}
bool QTOZWOptions::AppendLogFile() const {
    CALL_DPTR_PROP(AppendLogFile(), false);
}
bool QTOZWOptions::ConsoleOutput() const {
    CALL_DPTR_PROP(ConsoleOutput(), false);
}
OptionList QTOZWOptions::SaveLogLevel() const {
    CALL_DPTR_PROP(SaveLogLevel(), OptionList());
}
OptionList QTOZWOptions::QueueLogLevel() const {
    CALL_DPTR_PROP(QueueLogLevel(), OptionList());
}
OptionList QTOZWOptions::DumpTriggerLevel() const {
    CALL_DPTR_PROP(DumpTriggerLevel(), OptionList());
}
bool QTOZWOptions::Associate() const {
    CALL_DPTR_PROP(Associate(), false);
}
QString QTOZWOptions::Exclude() const {
    CALL_DPTR_PROP(Exclude(), QString());
}
QString QTOZWOptions::Include() const {
    CALL_DPTR_PROP(Include(), QString());
}
bool QTOZWOptions::NotifyTransactions() const {
    CALL_DPTR_PROP(NotifyTransactions(), false);
}
QString QTOZWOptions::Interface() const {
    CALL_DPTR_PROP(Interface(), QString());
}
bool QTOZWOptions::SaveConfiguration() const {
    CALL_DPTR_PROP(SaveConfiguration(), false);
}
qint32 QTOZWOptions::DriverMaxAttempts() const {
    CALL_DPTR_PROP(DriverMaxAttempts(), 0);
}
qint32 QTOZWOptions::PollInterval() const {
    CALL_DPTR_PROP(PollInterval(), 0);
}
bool QTOZWOptions::SuppressValueRefresh() const {
    CALL_DPTR_PROP(SuppressValueRefresh(), false);
}
bool QTOZWOptions::IntervalBetweenPolls() const {
    CALL_DPTR_PROP(IntervalBetweenPolls(), false);
}
bool QTOZWOptions::PerformReturnRoutes() const {
    CALL_DPTR_PROP(PerformReturnRoutes(), false);
}
QString QTOZWOptions::NetworkKey() const {
    CALL_DPTR_PROP(NetworkKey(), QString());
}
bool QTOZWOptions::RefreshAllUserCodes() const {
    CALL_DPTR_PROP(RefreshAllUserCodes(), false);
}
qint32 QTOZWOptions::RetryTimeout() const {
    CALL_DPTR_PROP(RetryTimeout(), 0);
}
bool QTOZWOptions::EnableSIS() const {
    CALL_DPTR_PROP(EnableSIS(), false);
}
bool QTOZWOptions::AssumeAwake() const {
    CALL_DPTR_PROP(AssumeAwake(), false);
}
bool QTOZWOptions::NotifyOnDriverUnload() const {
    CALL_DPTR_PROP(NotifyTransactions(), false);
}
OptionList QTOZWOptions::SecurityStrategy() const {
    CALL_DPTR_PROP(SecurityStrategy(), OptionList());
}
QString QTOZWOptions::CustomSecuredCC() const {
    CALL_DPTR_PROP(CustomSecuredCC(), QString());
}
bool QTOZWOptions::EnforceSecureReception() const {
    CALL_DPTR_PROP(EnforceSecureReception(), false);
}
bool QTOZWOptions::AutoUpdateConfigFile() const {
    CALL_DPTR_PROP(AutoUpdateConfigFile(), false);
}
OptionList QTOZWOptions::ReloadAfterUpdate() const {
    CALL_DPTR_PROP(ReloadAfterUpdate(), OptionList());
}
QString QTOZWOptions::Language() const {
    CALL_DPTR_PROP(Language(), QString());
}
bool QTOZWOptions::IncludeInstanceLabels() const {
    CALL_DPTR_PROP(IncludeInstanceLabels(), false);
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
void QTOZWOptions::setSecurityStrategy(OptionList SecurityStrategy) {
    CALL_DPTR_PROP_SET_TYPE(SecurityStrategy, OptionList);
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
void QTOZWOptions::setReloadAfterUpdate(OptionList ReloadAfterUpdate) {
    CALL_DPTR_PROP_SET_TYPE(ReloadAfterUpdate, OptionList);
}
void QTOZWOptions::setLanguage(QString Language) {
    CALL_DPTR_PROP_SET(Language);
}
void QTOZWOptions::setIncludeInstanceLabels(bool IncludeInstanceLabels) {
    CALL_DPTR_PROP_SET(IncludeInstanceLabels);
}
bool QTOZWOptions::AddOptionBool(QString option, bool value) {
    CALL_DPTR_RTN(AddOptionBool(option, value), bool, false);
}
bool QTOZWOptions::AddOptionInt(QString option, qint32 value) {
    CALL_DPTR_RTN(AddOptionInt(option, value), bool, false)
}
bool QTOZWOptions::AddOptionString(QString option, QString value) {
    CALL_DPTR_RTN(AddOptionString(option, value), bool, false)
}
bool QTOZWOptions::GetOptionAsBool(QString option) {
    CALL_DPTR_RTN(GetOptionAsBool(option), bool, false);
}
qint32 QTOZWOptions::GetOptionAsInt(QString option) {
    CALL_DPTR_RTN(GetOptionAsInt(option), qint32, 0);
}
QString QTOZWOptions::GetOptionAsString(QString option) {
    CALL_DPTR_RTN(GetOptionAsString(option), QString, QString());
}
bool QTOZWOptions::isLocked() {
    CALL_DPTR_RTN(isLocked(), bool, false);
}
