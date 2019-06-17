#ifndef QTOZWOPTIONS_H
#define QTOZWOPTIONS_H

#include <QObject>
#include <QtRemoteObjects>


class QTOZWOptions_Internal;
class QTOZWOptionsReplica;
class QTOZWManager;

class QTOZWOptions : public QObject {
    friend class QTOZWManager;


    Q_OBJECT
public:
    enum connectionType {
        Local,
        Remote,
        Invalid
    };
    Q_ENUM(connectionType)

    QTOZWOptions(connectionType type, QObject *parent = nullptr);

    Q_PROPERTY(QString ConfigPath READ ConfigPath WRITE setConfigPath NOTIFY ConfigPathChanged)
    Q_PROPERTY(QString UserPath READ UserPath WRITE setUserPath NOTIFY UserPathChanged)
    Q_PROPERTY(bool Logging READ Logging WRITE setLogging NOTIFY LoggingChanged)
    Q_PROPERTY(QString LogFileName READ LogFileName WRITE setLogFileName NOTIFY LogFileNameChanged)
    Q_PROPERTY(bool AppendLogFile READ AppendLogFile WRITE setAppendLogFile NOTIFY AppendLogFileChanged)
    Q_PROPERTY(bool ConsoleOutput READ ConsoleOutput WRITE setConsoleOutput NOTIFY ConsoleOutputChanged)
    Q_PROPERTY(qint32 SaveLogLevel READ SaveLogLevel WRITE setSaveLogLevel NOTIFY SaveLogLevelChanged)
    Q_PROPERTY(qint32 QueueLogLevel READ QueueLogLevel WRITE setQueueLogLevel NOTIFY QueueLogLevelChanged)
    Q_PROPERTY(qint32 DumpTriggerLevel READ DumpTriggerLevel WRITE setDumpTriggerLevel NOTIFY DumpTriggerLevelChanged)
    Q_PROPERTY(bool Associate READ Associate WRITE setAssociate NOTIFY AssociateChanged)
    Q_PROPERTY(QString Exclude READ Exclude WRITE setExclude NOTIFY ExcludeChanged)
    Q_PROPERTY(QString Include READ Include WRITE setInclude NOTIFY IncludeChanged)
    Q_PROPERTY(bool NotifyTransactions READ NotifyTransactions WRITE setNotifyTransactions NOTIFY NotifyTransactionsChanged)
    Q_PROPERTY(QString Interface READ Interface WRITE setInterface NOTIFY InterfaceChanged)
    Q_PROPERTY(bool SaveConfiguration READ SaveConfiguration WRITE setSaveConfiguration NOTIFY SaveConfigurationChanged)
    Q_PROPERTY(qint32 DriverMaxAttempts READ DriverMaxAttempts WRITE setDriverMaxAttempts NOTIFY DriverMaxAttemptsChanged)
    Q_PROPERTY(qint32 PollInterval READ PollInterval WRITE setPollInterval NOTIFY PollIntervalChanged)
    Q_PROPERTY(bool IntervalBetweenPolls READ IntervalBetweenPolls WRITE setIntervalBetweenPolls NOTIFY IntervalBetweenPollsChanged)
    Q_PROPERTY(bool SuppressValueRefresh READ SuppressValueRefresh WRITE setSuppressValueRefresh NOTIFY SuppressValueRefreshChanged)
    Q_PROPERTY(bool PerformReturnRoutes READ PerformReturnRoutes WRITE setPerformReturnRoutes NOTIFY PerformReturnRoutesChanged)
    Q_PROPERTY(QString NetworkKey READ NetworkKey WRITE setNetworkKey NOTIFY NetworkKeyChanged)
    Q_PROPERTY(bool RefreshAllUserCodes READ RefreshAllUserCodes WRITE setRefreshAllUserCodes NOTIFY RefreshAllUserCodesChanged)
    Q_PROPERTY(qint32 RetryTimeout READ RetryTimeout WRITE setRetryTimeout NOTIFY RetryTimeoutChanged)
    Q_PROPERTY(bool EnableSIS READ EnableSIS WRITE setEnableSIS NOTIFY EnableSISChanged)
    Q_PROPERTY(bool AssumeAwake READ AssumeAwake WRITE setAssumeAwake NOTIFY AssumeAwakeChanged)
    Q_PROPERTY(bool NotifyOnDriverUnload READ NotifyOnDriverUnload WRITE setNotifyOnDriverUnload NOTIFY NotifyOnDriverUnloadChanged)
    Q_PROPERTY(QString SecurityStrategy READ SecurityStrategy WRITE setSecurityStrategy NOTIFY SecurityStrategyChanged)
    Q_PROPERTY(QString CustomSecuredCC READ CustomSecuredCC WRITE setCustomSecuredCC NOTIFY CustomSecuredCCChanged)
    Q_PROPERTY(bool EnforceSecureReception READ EnforceSecureReception WRITE setEnforceSecureReception NOTIFY EnforceSecureReceptionChanged)
    Q_PROPERTY(bool AutoUpdateConfigFile READ AutoUpdateConfigFile WRITE setAutoUpdateConfigFile NOTIFY AutoUpdateConfigFileChanged)
    Q_PROPERTY(QString ReloadAfterUpdate READ ReloadAfterUpdate WRITE setReloadAfterUpdate NOTIFY ReloadAfterUpdateChanged)
    Q_PROPERTY(QString Language READ Language WRITE setLanguage NOTIFY LanguageChanged)
    Q_PROPERTY(bool IncludeInstanceLabels READ IncludeInstanceLabels WRITE setIncludeInstanceLabels NOTIFY IncludeInstanceLabelsChanged)

    QString ConfigPath() const;
    QString UserPath() const;
    bool Logging() const;
    QString LogFileName() const;
    bool AppendLogFile() const;
    bool ConsoleOutput() const;
    qint32 SaveLogLevel() const;
    qint32 QueueLogLevel() const;
    qint32 DumpTriggerLevel() const;
    bool Associate() const;
    QString Exclude() const;
    QString Include() const;
    bool NotifyTransactions() const;
    QString Interface() const;
    bool SaveConfiguration() const;
    qint32 DriverMaxAttempts() const;
    qint32 PollInterval() const;
    bool SuppressValueRefresh() const;
    bool IntervalBetweenPolls() const;
    bool PerformReturnRoutes() const ;
    QString NetworkKey() const;
    bool RefreshAllUserCodes() const;
    qint32 RetryTimeout() const;
    bool EnableSIS() const;
    bool AssumeAwake() const;
    bool NotifyOnDriverUnload() const;
    QString SecurityStrategy() const;
    QString CustomSecuredCC() const;
    bool EnforceSecureReception() const;
    bool AutoUpdateConfigFile() const;
    QString ReloadAfterUpdate() const;
    QString Language() const;
    bool IncludeInstanceLabels() const;

    void setConfigPath(QString ConfigPath);
    void setUserPath(QString UserPath);
    void setLogging(bool Logging);
    void setLogFileName(QString LogFileName);
    void setAppendLogFile(bool AppendLogFile);
    void setConsoleOutput(bool ConsoleOutput);
    void setSaveLogLevel(qint32 SaveLogLevel);
    void setQueueLogLevel(qint32 QueueLogLevel);
    void setDumpTriggerLevel(qint32 DumpTriggerLevel);
    void setAssociate(bool Associate);
    void setExclude(QString Exclude);
    void setInclude(QString Include);
    void setNotifyTransactions(bool NotifyTransactions);
    void setInterface(QString Interface);
    void setSaveConfiguration(bool SaveConfiguration);
    void setDriverMaxAttempts(qint32 DriverMaxAttempts);
    void setPollInterval(qint32 PollInterval);
    void setIntervalBetweenPolls(bool IntervalBetweenPolls);
    void setSuppressValueRefresh(bool SuppressValueRefresh);
    void setPerformReturnRoutes(bool PerformReturnRoutes);
    void setNetworkKey(QString NetworkKey);
    void setRefreshAllUserCodes(bool RefreshAllUserCodes);
    void setRetryTimeout(qint32 RetryTimeout);
    void setEnableSIS(bool EnableSIS);
    void setAssumeAwake(bool AssumeAwake);
    void setNotifyOnDriverUnload(bool NotifyOnDriverUnload);
    void setSecurityStrategy(QString SecurityStrategy);
    void setCustomSecuredCC(QString CustomSecuredCC);
    void setEnforceSecureReception(bool EnforceSecureReception);
    void setAutoUpdateConfigFile(bool AutoUpdateConfigFile);
    void setReloadAfterUpdate(QString ReloadAfterUpdate);
    void setLanguage(QString Language);
    void setIncludeInstanceLabels(bool IncludeInstanceLabels);

    bool initilizeBase();
    bool initilizeSource(QRemoteObjectHost *m_sourceNode);
    bool initilizeReplica(QRemoteObjectNode *m_replicaNode);

    bool AddOptionBool(QString option, bool value);
    bool AddOptionInt(QString option, qint32 value);
    bool AddOptionString(QString option, QString value);
    bool GetOptionAsBool(QString option);
    qint32 GetOptionAsInt(QString option);
    QString GetOptionAsString(QString option);
    bool isLocked();

Q_SIGNALS:
    void ConfigPathChanged(QString ConfigPath);
    void UserPathChanged(QString UserPath);
    void LoggingChanged(bool Logging);
    void LogFileNameChanged(QString LogFileName);
    void AppendLogFileChanged(bool AppendLogFile);
    void ConsoleOutputChanged(bool ConsoleOutput);
    void SaveLogLevelChanged(qint32 SaveLogLevel);
    void QueueLogLevelChanged(qint32 QueueLogLevel);
    void DumpTriggerLevelChanged(qint32 DumpTriggerLevel);
    void AssociateChanged(bool Associate);
    void ExcludeChanged(QString Exclude);
    void IncludeChanged(QString Include);
    void NotifyTransactionsChanged(bool NotifyTransactions);
    void InterfaceChanged(QString Interface);
    void SaveConfigurationChanged(bool SaveConfiguration);
    void DriverMaxAttemptsChanged(qint32 DriverMaxAttempts);
    void PollIntervalChanged(qint32 PollInterval);
    void IntervalBetweenPollsChanged(bool IntervalBetweenPolls);
    void SuppressValueRefreshChanged(bool SuppressValueRefresh);
    void PerformReturnRoutesChanged(bool PerformReturnRoutes);
    void NetworkKeyChanged(QString NetworkKey);
    void RefreshAllUserCodesChanged(bool RefreshAllUserCodes);
    void RetryTimeoutChanged(qint32 RetryTimeout);
    void EnableSISChanged(bool EnableSIS);
    void AssumeAwakeChanged(bool AssumeAwake);
    void NotifyOnDriverUnloadChanged(bool NotifyOnDriverUnload);
    void SecurityStrategyChanged(QString SecurityStrategy);
    void CustomSecuredCCChanged(QString CustomSecuredCC);
    void EnforceSecureReceptionChanged(bool EnforceSecureReception);
    void AutoUpdateConfigFileChanged(bool AutoUpdateConfigFile);
    void ReloadAfterUpdateChanged(QString ReloadAfterUpdate);
    void LanguageChanged(QString Language);
    void IncludeInstanceLabelsChanged(bool IncludeInstanceLabels);
    //void error(QTOZWOptionsReplica::QTOZWOptionsErrorCodes errorcode);
    void onOptionStateChange(QRemoteObjectReplica::State);

private:
    void connectSignals();


    QTOZWOptions_Internal *d_ptr_internal;
    QTOZWOptionsReplica *d_ptr_replica;
    connectionType m_connectionType;
    QRemoteObjectReplica::State m_optionState;
};



#endif // QTOZWOPTIONS_H
