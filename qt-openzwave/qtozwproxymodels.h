#ifndef QTOZWPROXYMODELS_H
#define QTOZWPROXYMODELS_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>

#include "qtozwnodemodel.h"
#include "qtozwvalueidmodel.h"

class QTOZW_proxyNodeModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit QTOZW_proxyNodeModel(QObject *parent = nullptr);

signals:

public slots:
};


class QTOZW_proxyValueModel : public QSortFilterProxyModel
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


class QTOZW_proxyAssociationModel : public QSortFilterProxyModel
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
