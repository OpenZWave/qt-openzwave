#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>
#include <QPainter>
#include <QDebug>
#include "qtozw_itemdelegate.h"
#include "qtozwvalueidmodel.h"

QTOZW_ItemDelegate::QTOZW_ItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    this->m_comboBox = new QComboBox();
    this->m_comboBox->setFrame(false);
}

void QTOZW_ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QModelIndex typeIndex = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Type);
    if (typeIndex.data() != QTOZW_ValueIds::ValueIdTypes::List) {
        return QStyledItemDelegate::paint(painter, option, index);
    }
    QModelIndex flagsIndex = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::ValueFlags);
    QTOZW_ValueIDBitSet flags = flagsIndex.data().value<QTOZW_ValueIDBitSet>();
    QTOZW_ValueIDList val = index.data().value<QTOZW_ValueIDList>();
    this->m_comboBox->addItems(val.labels);
    this->m_comboBox->setCurrentText(val.selectedItem);
    this->m_comboBox->resize(option.rect.size());
    qDebug() << flags.values;
//    if (flags.values.at(QTOZW_ValueIds::ValueIDFlags::ReadOnly) == true)
//        this->m_comboBox->setEnabled(false);
    painter->save();
    painter->translate(option.rect.topLeft());
    this->m_comboBox->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);
    painter->restore();
}

QSize QTOZW_ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return this->m_comboBox->minimumSizeHint();
}
