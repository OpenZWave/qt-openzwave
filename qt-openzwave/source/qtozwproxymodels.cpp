#include <QDebug>
#include "qt-openzwave/qtozwproxymodels.h"
#include "qt-openzwave/qtozwvalueidmodel.h"
#include "qt-openzwave/qtozwnodemodel.h"
#include "qt-openzwave/qtozwassociationmodel.h"


QTOZW_proxyNodeModel::QTOZW_proxyNodeModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

QTOZW_proxyValueModel::QTOZW_proxyValueModel(QObject *parent) :
    QSortFilterProxyModel (parent),
    m_genreFilter(QTOZW_ValueIds::ValueIdGenres::GenreCount)
{

}

void QTOZW_proxyValueModel::setSelectionModel(QItemSelectionModel *model) {
    this->m_selectionmodel = model;
    QObject::connect(this->m_selectionmodel, &QItemSelectionModel::currentRowChanged, this, &QTOZW_proxyValueModel::currentSelectedRowChanged);
}

void QTOZW_proxyValueModel::currentSelectedRowChanged(const QModelIndex &current, const QModelIndex &previous) {
    Q_UNUSED(previous);
    this->m_nodeFilter = current.siblingAtColumn(QTOZW_Nodes::NodeColumns::NodeID).data().value<quint8>();
    invalidateFilter();
}
void QTOZW_proxyValueModel::setFilterGenre(QTOZW_ValueIds::ValueIdGenres genre)
{
    this->m_genreFilter = genre;
    invalidateFilter();
}

bool QTOZW_proxyValueModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    if (this->m_genreFilter != QTOZW_ValueIds::ValueIdGenres::GenreCount) {
        QModelIndex genreIndex = sourceModel()->index(sourceRow, QTOZW_ValueIds::ValueIdColumns::Genre, sourceParent);
        if (genreIndex.data().value<QTOZW_ValueIds::ValueIdGenres>() != this->m_genreFilter)
            return false;
    }
    if (this->m_nodeFilter > 0) {
        QModelIndex nodeIDIndex = sourceModel()->index(sourceRow, QTOZW_ValueIds::ValueIdColumns::Node, sourceParent);
        if (nodeIDIndex.data().value<quint8>() != this->m_nodeFilter)
            return false;
    }
    return true;
}

QTOZW_proxyAssociationModel::QTOZW_proxyAssociationModel(QObject *parent) :
    QSortFilterProxyModel (parent)
{

}

void QTOZW_proxyAssociationModel::setSelectionModel(QItemSelectionModel *model) {
    this->m_selectionmodel = model;
    QObject::connect(this->m_selectionmodel, &QItemSelectionModel::currentRowChanged, this, &QTOZW_proxyAssociationModel::currentSelectedRowChanged);
}

void QTOZW_proxyAssociationModel::currentSelectedRowChanged(const QModelIndex &current, const QModelIndex &previous) {
    Q_UNUSED(previous);
    this->m_nodeFilter = current.siblingAtColumn(QTOZW_Nodes::NodeColumns::NodeID).data().value<quint8>();
    invalidateFilter();
}

bool QTOZW_proxyAssociationModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    if (this->m_nodeFilter > 0) {
        QModelIndex nodeIDIndex = sourceModel()->index(sourceRow, QTOZW_Associations::associationColumns::NodeID, sourceParent);
        if (nodeIDIndex.data().value<quint8>() != this->m_nodeFilter)
            return false;
    }
    return true;
}

