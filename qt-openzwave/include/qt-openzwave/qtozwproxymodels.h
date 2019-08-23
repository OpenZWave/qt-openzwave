//-----------------------------------------------------------------------------
//
//	qtozwproxymodels.h
//
//	Various useful Filtering Proxy Models for QT-OpenZWave
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

#ifndef QTOZWPROXYMODELS_H
#define QTOZWPROXYMODELS_H

#include "qt-openzwave/qtopenzwave.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>

#include "qtozwnodemodel.h"
#include "qtozwvalueidmodel.h"

class QTOPENZWAVESHARED_EXPORT QTOZW_proxyNodeModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit QTOZW_proxyNodeModel(QObject *parent = nullptr);

signals:

public slots:
};


class QTOPENZWAVESHARED_EXPORT QTOZW_proxyValueModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit QTOZW_proxyValueModel(QObject *parent = nullptr);
    void setSelectionModel(QItemSelectionModel *);
    void setFilterGenre(QTOZW_ValueIds::ValueIdGenres genre);
    //    setNodeView(QAbstractItemView *);

public slots:
    void currentSelectedRowChanged(const QModelIndex &current, const QModelIndex &previous);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QItemSelectionModel *m_selectionmodel;
    QTOZW_ValueIds::ValueIdGenres m_genreFilter;
    uint8_t m_nodeFilter;
};


class QTOPENZWAVESHARED_EXPORT QTOZW_proxyAssociationModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit QTOZW_proxyAssociationModel(QObject *parent = nullptr);
    void setSelectionModel(QItemSelectionModel *);

public slots:
    void currentSelectedRowChanged(const QModelIndex &current, const QModelIndex &previous);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QItemSelectionModel *m_selectionmodel;
    uint8_t m_nodeFilter;
};




#endif // QTOZWPROXYMODELS_H
