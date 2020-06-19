//-----------------------------------------------------------------------------
//
//	qtozwoptions_p.cpp
//
//	OpenZWave Options Wrapper - Internal Class to interface with it
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

#include "qtozwoptions_p.h"
#include "platform/Log.h"
#include "qtozw_logging.h"


QTOZWOptions_Internal::QTOZWOptions_Internal(QString localConfigPath, QString localUserPath, QObject *parent)
    : QTOZWOptionsSimpleSource (parent),
      m_updating(false),
      m_localConfigPath(localConfigPath),
      m_localUserPath(localUserPath)
{
    this->setObjectName("QTOZW_Options");

    this->m_options = OpenZWave::Options::Get();
    if (!this->m_options) {
        this->m_options = OpenZWave::Options::Create(QFileInfo(m_localConfigPath.append("/")).absoluteFilePath().toStdString(), QFileInfo(m_localUserPath.append("/")).absoluteFilePath().toStdString(), "");
    }


    this->populateProperties();

    connect(this, &QTOZWOptionsSimpleSource::ConfigPathChanged, this, &QTOZWOptions_Internal::pvt_ConfigPathChanged);
    connect(this, &QTOZWOptionsSimpleSource::UserPathChanged, this, &QTOZWOptions_Internal::pvt_UserPathChanged);
    connect(this, &QTOZWOptionsSimpleSource::LoggingChanged, this, &QTOZWOptions_Internal::pvt_LoggingChanged);
    connect(this, &QTOZWOptionsSimpleSource::LogFileNameChanged, this, &QTOZWOptions_Internal::pvt_LogFileNameChanged);
    connect(this, &QTOZWOptionsSimpleSource::AppendLogFileChanged, this, &QTOZWOptions_Internal::pvt_AppendLogFileChanged);
    connect(this, &QTOZWOptionsSimpleSource::ConsoleOutputChanged, this, &QTOZWOptions_Internal::pvt_ConsoleOutputChanged);
    connect(this, &QTOZWOptionsSimpleSource::SaveLogLevelChanged, this, &QTOZWOptions_Internal::pvt_SaveLogLevelChanged);
    connect(this, &QTOZWOptionsSimpleSource::QueueLogLevelChanged, this, &QTOZWOptions_Internal::pvt_QueueLogLevelChanged);
    connect(this, &QTOZWOptionsSimpleSource::DumpTriggerLevelChanged, this, &QTOZWOptions_Internal::pvt_DumpTriggerLevelChanged);
    connect(this, &QTOZWOptionsSimpleSource::AssociateChanged, this, &QTOZWOptions_Internal::pvt_AssociateChanged);
    connect(this, &QTOZWOptionsSimpleSource::ExcludeChanged, this, &QTOZWOptions_Internal::pvt_ExcludeChanged);
    connect(this, &QTOZWOptionsSimpleSource::IncludeChanged, this, &QTOZWOptions_Internal::pvt_IncludeChanged);
    connect(this, &QTOZWOptionsSimpleSource::NotifyTransactionsChanged, this, &QTOZWOptions_Internal::pvt_NotifyTransactionChanged);
    connect(this, &QTOZWOptionsSimpleSource::InterfaceChanged, this, &QTOZWOptions_Internal::pvt_InterfaceChanged);
    connect(this, &QTOZWOptionsSimpleSource::SaveConfigurationChanged, this, &QTOZWOptions_Internal::pvt_SaveConfigurationChanged);
    connect(this, &QTOZWOptionsSimpleSource::DriverMaxAttemptsChanged, this, &QTOZWOptions_Internal::pvt_DriverMaxAttemptsChanged);
    connect(this, &QTOZWOptionsSimpleSource::PollIntervalChanged, this, &QTOZWOptions_Internal::pvt_PollIntervalChanged);
    connect(this, &QTOZWOptionsSimpleSource::IntervalBetweenPollsChanged, this, &QTOZWOptions_Internal::pvt_IntervalBetweenPollsChanged);
    connect(this, &QTOZWOptionsSimpleSource::SuppressValueRefreshChanged, this, &QTOZWOptions_Internal::pvt_SuppressValueRefreshChanged);
    connect(this, &QTOZWOptionsSimpleSource::PerformReturnRoutesChanged, this, &QTOZWOptions_Internal::pvt_PerformReturnRoutesChanged);
    connect(this, &QTOZWOptionsSimpleSource::NetworkKeyChanged, this, &QTOZWOptions_Internal::pvt_NetworkKeyChanged);
    connect(this, &QTOZWOptionsSimpleSource::RefreshAllUserCodesChanged, this, &QTOZWOptions_Internal::pvt_RefreshAllUserCodesChanged);
    connect(this, &QTOZWOptionsSimpleSource::RetryTimeoutChanged, this, &QTOZWOptions_Internal::pvt_RetryTimeoutChanged);
    connect(this, &QTOZWOptionsSimpleSource::EnableSISChanged, this, &QTOZWOptions_Internal::pvt_EnableSISChanged);
    connect(this, &QTOZWOptionsSimpleSource::AssumeAwakeChanged, this, &QTOZWOptions_Internal::pvt_AssumeAwakeChanged);
    connect(this, &QTOZWOptionsSimpleSource::NotifyOnDriverUnloadChanged, this, &QTOZWOptions_Internal::pvt_NotifyOnDriverUnloadChanged);
    connect(this, &QTOZWOptionsSimpleSource::SecurityStrategyChanged, this, &QTOZWOptions_Internal::pvt_SecurityStrategyChanged);
    connect(this, &QTOZWOptionsSimpleSource::CustomSecuredCCChanged, this, &QTOZWOptions_Internal::pvt_CustomSecuredCCChanged);
    connect(this, &QTOZWOptionsSimpleSource::EnforceSecureReceptionChanged, this, &QTOZWOptions_Internal::pvt_EnforceSecureReceptionChanged);
    connect(this, &QTOZWOptionsSimpleSource::AutoUpdateConfigFileChanged, this, &QTOZWOptions_Internal::pvt_AutoUpdateConfigFileChanged);
    connect(this, &QTOZWOptionsSimpleSource::ReloadAfterUpdateChanged, this, &QTOZWOptions_Internal::pvt_ReloadAfterUpdateChanged);
    connect(this, &QTOZWOptionsSimpleSource::LanguageChanged, this, &QTOZWOptions_Internal::pvt_LanguageChanged);
    connect(this, &QTOZWOptionsSimpleSource::IncludeInstanceLabelsChanged, this, &QTOZWOptions_Internal::pvt_IncludeInstanceLabelsChanged);

}
bool QTOZWOptions_Internal::AddOptionBool(QString option, bool value) {
    return this->m_options->AddOptionBool(option.toStdString(), value);
}
bool QTOZWOptions_Internal::AddOptionInt(QString option, qint32 value) {
    return this->m_options->AddOptionInt(option.toStdString(), value);
}
bool QTOZWOptions_Internal::AddOptionString(QString option, QString value) {
    return this->m_options->AddOptionString(option.toStdString(), value.toStdString(), false);
}
bool QTOZWOptions_Internal::GetOptionAsBool(QString option) {
    bool value;
    if (this->m_options->GetOptionAsBool(option.toStdString(), &value)) {
        return value;
    }
    emit this->error(QTOZWOptionsErrorCodes::InvalidOption);
    return false;
}
qint32 QTOZWOptions_Internal::GetOptionAsInt(QString option) {
    qint32 value;
    if (this->m_options->GetOptionAsInt(option.toStdString(), &value)) {
        return value;
    }
    emit this->error(QTOZWOptionsErrorCodes::InvalidOption);
    return -1;
}
QString QTOZWOptions_Internal::GetOptionAsString(QString option) {
    std::string value;
    if (this->m_options->GetOptionAsString(option.toStdString(), &value)) {
        return QString::fromStdString(value);
    }
    emit this->error(QTOZWOptionsErrorCodes::InvalidOption);
    return QString();
}

bool QTOZWOptions_Internal::isLocked() {
    return this->m_options->AreLocked();
}


bool QTOZWOptions_Internal::populateProperties() {
    this->m_updating = true;
    this->setConfigPath(this->GetOptionAsString("ConfigPath"));
    this->setUserPath(this->GetOptionAsString("UserPath"));
    this->setLogging(this->GetOptionAsBool("Logging"));
    this->setLogFileName(this->GetOptionAsString("LogFileName"));
    this->setAppendLogFile(this->GetOptionAsBool("AppendLogFile"));
    this->setConsoleOutput(this->GetOptionAsBool("ConsoleOutput"));
    {
        OptionList saveLevel;
        saveLevel.setEnums(QStringList() << "None" << "Always" << "Fatal" << "Error" << "Warning" << "Alert" << "Info" << "Detail" << "Debug");
        int level = this->GetOptionAsInt("SaveLogLevel");
        if ((level >= OpenZWave::LogLevel::LogLevel_None) && (level <= OpenZWave::LogLevel::LogLevel_Debug))
            saveLevel.setSelected(level);
        else {
            saveLevel.setSelected(0);
        }
        this->setSaveLogLevel(saveLevel);
    }
    {
        OptionList queueLevel;
        queueLevel.setEnums(QStringList() << "None" << "Always" << "Fatal" << "Error" << "Warning" << "Alert" << "Info" << "Detail" << "Debug");
        int level = this->GetOptionAsInt("QueueLogLevel");
        if ((level >= OpenZWave::LogLevel::LogLevel_None) && (level <= OpenZWave::LogLevel::LogLevel_Debug))
            queueLevel.setSelected(level);
        else {
            queueLevel.setSelected(0);
        }
        this->setQueueLogLevel(queueLevel);
    }
    {
        OptionList dumpLevel;
        dumpLevel.setEnums(QStringList() << "None" << "Always" << "Fatal" << "Error" << "Warning" << "Alert" << "Info" << "Detail" << "Debug");
        int level = this->GetOptionAsInt("DumpTriggerLevel");
        if ((level >= OpenZWave::LogLevel::LogLevel_None) && (level <= OpenZWave::LogLevel::LogLevel_Debug))
            dumpLevel.setSelected(level);
        else {
            dumpLevel.setSelected(0);
        }
        this->setDumpTriggerLevel(dumpLevel);
    }

    this->setAssociate(this->GetOptionAsBool("Associate"));
    this->setExclude(this->GetOptionAsString("Exclude"));
    this->setInclude(this->GetOptionAsString("Include"));
    this->setNotifyTransactions(this->GetOptionAsBool("NotifyTransactions"));
    this->setInterface(this->GetOptionAsString("Interface"));
    this->setSaveConfiguration(this->GetOptionAsBool("SaveConfiguration"));
    this->setDriverMaxAttempts(this->GetOptionAsInt("DriverMaxAttempts"));
    this->setPollInterval(this->GetOptionAsInt("PollInterval"));
    this->setIntervalBetweenPolls(this->GetOptionAsBool("IntervalBetweenPolls"));
    this->setSuppressValueRefresh(this->GetOptionAsBool("SuppressValueRefresh"));
    this->setPerformReturnRoutes(this->GetOptionAsBool("PerformReturnRoutes"));
    this->setNetworkKey(this->GetOptionAsString("NetworkKey"));
    this->setRefreshAllUserCodes(this->GetOptionAsBool("RefreshAllUserCodes"));
    this->setRetryTimeout(this->GetOptionAsInt("RetryTimeout"));
    this->setEnableSIS(this->GetOptionAsBool("EnableSIS"));
    this->setAssumeAwake(this->GetOptionAsBool("AssumeAwake"));
    this->setNotifyOnDriverUnload(this->GetOptionAsBool("NotifyOnDriverUnload"));
    {
        OptionList secstrategy;
        secstrategy.setEnums(QStringList() << "Essential" << "Supported" << "Custom");
        QString strategy = this->GetOptionAsString("SecurityStrategy");
        secstrategy.setSelected(strategy);
        this->setSecurityStrategy(secstrategy);
    }
    this->setCustomSecuredCC(this->GetOptionAsString("CustomSecuredCC"));
    this->setEnforceSecureReception(this->GetOptionAsBool("EnforceSecureReception"));
    this->setAutoUpdateConfigFile(this->GetOptionAsBool("AutoUpdateConfigFile"));
    {
        OptionList reloadoption;
        reloadoption.setEnums(QStringList() << "Never" << "Immediate" << "Awake");
        QString strategy = this->GetOptionAsString("ReloadAfterUpdate");
        reloadoption.setSelected(strategy);
        this->setReloadAfterUpdate(reloadoption);
    }
    this->setLanguage(this->GetOptionAsString("Language"));
    this->setIncludeInstanceLabels(this->GetOptionAsBool("IncludeInstanceLabels"));
    this->m_updating = false;
    return true;
}

void QTOZWOptions_Internal::pvt_ConfigPathChanged(QString value) {
    if (this->m_updating)
        return;
    /* OZW expects the paths to end with a / otherwise it treats it as a file */
    if (value.at(value.size()-1) != "/")
        value.append("/");
    this->m_options->AddOptionString("ConfigPath", value.toStdString(), false);
}
void QTOZWOptions_Internal::pvt_UserPathChanged(QString value) {
    if (this->m_updating)
        return;
    /* OZW expects the paths to end with a / otherwise it treats it as a file */
    if (value.at(value.size()-1) != "/")
        value.append("/");
    this->m_options->AddOptionString("UserPath", value.toStdString(), false);
}
void QTOZWOptions_Internal::pvt_LoggingChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("Logging", value);
}
void QTOZWOptions_Internal::pvt_LogFileNameChanged(QString value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionString("LogFileName", value.toStdString(), false);
}
void QTOZWOptions_Internal::pvt_AppendLogFileChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("AppendLogFile", value);
}
void QTOZWOptions_Internal::pvt_ConsoleOutputChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("ConsoleOutput", value);
}
void QTOZWOptions_Internal::pvt_SaveLogLevelChanged(OptionList value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionInt("SaveLogLevel", value.getSelected());
}
void QTOZWOptions_Internal::pvt_QueueLogLevelChanged(OptionList value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionInt("QueueLogLevel", value.getSelected());
}
void QTOZWOptions_Internal::pvt_DumpTriggerLevelChanged(OptionList value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionInt("DumpTriggerLevel", value.getSelected());
}
void QTOZWOptions_Internal::pvt_AssociateChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("Associate", value);
}
void QTOZWOptions_Internal::pvt_ExcludeChanged(QString value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionString("Exclude", value.toStdString(), false);
}
void QTOZWOptions_Internal::pvt_IncludeChanged(QString value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionString("Include", value.toStdString(), false);
}
void QTOZWOptions_Internal::pvt_NotifyTransactionChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("NotifyTransaction", value);
}
void QTOZWOptions_Internal::pvt_InterfaceChanged(QString value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionString("Interface", value.toStdString(), false);
}
void QTOZWOptions_Internal::pvt_SaveConfigurationChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("SaveConfiguration", value);
}
void QTOZWOptions_Internal::pvt_DriverMaxAttemptsChanged(qint32 value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionInt("DriverMaxAttempts", value);
}
void QTOZWOptions_Internal::pvt_PollIntervalChanged(qint32 value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionInt("PollInterval", value);
}
void QTOZWOptions_Internal::pvt_IntervalBetweenPollsChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("IntervalBetweenPolls", value);
}
void QTOZWOptions_Internal::pvt_SuppressValueRefreshChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("SuppressValueRefresh", value);
}
void QTOZWOptions_Internal::pvt_PerformReturnRoutesChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("PerformReturnRoutes", value);
}
void QTOZWOptions_Internal::pvt_NetworkKeyChanged(QString value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionString("NetworkKey", value.toStdString(), false);
}
void QTOZWOptions_Internal::pvt_RefreshAllUserCodesChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("RefreshAllUserCodes", value);
}
void QTOZWOptions_Internal::pvt_RetryTimeoutChanged(qint32 value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionInt("RetryTimeout", value);
}
void QTOZWOptions_Internal::pvt_EnableSISChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("EnableSIS", value);
}
void QTOZWOptions_Internal::pvt_AssumeAwakeChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("AssumeAwake", value);
}
void QTOZWOptions_Internal::pvt_NotifyOnDriverUnloadChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("NotifyOnDriverUnload", value);
}
void QTOZWOptions_Internal::pvt_SecurityStrategyChanged(OptionList value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionString("SecurityStrategy", value.getSelectedName().toStdString(), false);
}
void QTOZWOptions_Internal::pvt_CustomSecuredCCChanged(QString value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionString("CustomSecuredCC", value.toStdString(), false);
}
void QTOZWOptions_Internal::pvt_EnforceSecureReceptionChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("EnforceSecureReception", value);
}
void QTOZWOptions_Internal::pvt_AutoUpdateConfigFileChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("AutoUpdateConfigFile", value);
}
void QTOZWOptions_Internal::pvt_ReloadAfterUpdateChanged(OptionList value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionString("ReloadAfterUpdate", value.getSelectedName().toStdString(), false);
}
void QTOZWOptions_Internal::pvt_LanguageChanged(QString value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionString("Language", value.toStdString(), false);
}
void QTOZWOptions_Internal::pvt_IncludeInstanceLabelsChanged(bool value) {
    if (this->m_updating)
        return;
    this->m_options->AddOptionBool("IncludeInstanceLabels", value);
}

