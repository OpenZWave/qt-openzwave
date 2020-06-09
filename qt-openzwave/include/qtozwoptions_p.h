//-----------------------------------------------------------------------------
//
//	qtozwoptions_p.h
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

#ifndef QTOZWOPTIONS_P_H
#define QTOZWOPTIONS_P_H

#include "qt-openzwave/qtopenzwave.h"

#include <QObject>
#include <QUrl>
#include <Options.h>
#include "qtozw_logging.h"
#include "rep_qtozwoptions_source.h"
#include "rep_qtozwoptions_replica.h"


class QTOZWOptions_Internal : public QTOZWOptionsSimpleSource
{
    Q_OBJECT
public:
    QTOZWOptions_Internal(QString localConfigPath, QString localUserPath, QObject *parent = nullptr);

public Q_SLOTS:
    bool AddOptionBool(QString option, bool value);
    bool AddOptionInt(QString option, qint32 value);
    bool AddOptionString(QString option, QString value);
    bool GetOptionAsBool(QString option);
    qint32 GetOptionAsInt(QString option);
    QString GetOptionAsString(QString option);
    bool isLocked();

private Q_SLOTS:
    void pvt_ConfigPathChanged(QString value);
    void pvt_UserPathChanged(QString value);
    void pvt_LoggingChanged(bool value);
    void pvt_LogFileNameChanged(QString value);
    void pvt_AppendLogFileChanged(bool value);
    void pvt_ConsoleOutputChanged(bool value);
    void pvt_SaveLogLevelChanged(OptionList value);
    void pvt_QueueLogLevelChanged(OptionList value);
    void pvt_DumpTriggerLevelChanged(OptionList value);
    void pvt_AssociateChanged(bool value);
    void pvt_ExcludeChanged(QString value);
    void pvt_IncludeChanged(QString value);
    void pvt_NotifyTransactionChanged(bool value);
    void pvt_InterfaceChanged(QString value);
    void pvt_SaveConfigurationChanged(bool value);
    void pvt_DriverMaxAttemptsChanged(qint32 value);
    void pvt_PollIntervalChanged(qint32 value);
    void pvt_IntervalBetweenPollsChanged(bool value);
    void pvt_SuppressValueRefreshChanged(bool value);
    void pvt_PerformReturnRoutesChanged(bool value);
    void pvt_NetworkKeyChanged(QString value);
    void pvt_RefreshAllUserCodesChanged(bool value);
    void pvt_RetryTimeoutChanged(qint32 value);
    void pvt_EnableSISChanged(bool value);
    void pvt_AssumeAwakeChanged(bool value);
    void pvt_NotifyOnDriverUnloadChanged(bool value);
    void pvt_SecurityStrategyChanged(OptionList value);
    void pvt_CustomSecuredCCChanged(QString value);
    void pvt_EnforceSecureReceptionChanged(bool value);
    void pvt_AutoUpdateConfigFileChanged(bool value);
    void pvt_ReloadAfterUpdateChanged(OptionList value);
    void pvt_LanguageChanged(QString value);
    void pvt_IncludeInstanceLabelsChanged(bool value);

private:
    bool populateProperties();
    bool m_updating;
    OpenZWave::Options *m_options;
    QString m_localConfigPath;
    QString m_localUserPath;
};

#endif // QTOZWOPTIONS_P_H
