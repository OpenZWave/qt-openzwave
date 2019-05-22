#include <QPainter>
#include <QDebug>
#include "qtozw_itemdelegate.h"
#include "qtozwvalueidmodel.h"

QTOZW_ItemDelegate::QTOZW_ItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    this->m_comboBox = new QComboBox();
    this->m_checkBox = new QCheckBox();
    this->m_spinBox = new QSpinBox();
}

void QTOZW_ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QModelIndex typeIndex = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Type);
    QModelIndex flagsIndex = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::ValueFlags);
    QBitArray flags = flagsIndex.data().value<QBitArray>();
    bool readOnly = flags.at(QTOZW_ValueIds::ValueIDFlags::ReadOnly);

    switch (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>()) {
        case QTOZW_ValueIds::ValueIdTypes::List: {
            QTOZW_ValueIDList val = index.data().value<QTOZW_ValueIDList>();
            this->m_comboBox->setFrame(false);
            this->m_comboBox->addItems(val.labels);
            this->m_comboBox->setCurrentText(val.selectedItem);
            this->m_comboBox->resize(option.rect.size());
            this->m_comboBox->setEnabled(!readOnly);
            painter->save();
            painter->translate(option.rect.topLeft());
            this->m_comboBox->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);
            painter->restore();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            this->m_checkBox->setChecked(index.data().value<bool>());
            this->m_checkBox->setEnabled(!readOnly);
            this->m_checkBox->resize(option.rect.size());
            painter->save();
            painter->translate(option.rect.topLeft());
            this->m_checkBox->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);
            painter->restore();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Int:
        case QTOZW_ValueIds::ValueIdTypes::Byte:
        case QTOZW_ValueIds::ValueIdTypes::Short: {
            this->m_spinBox->setValue(index.data().toInt());
            this->m_spinBox->setEnabled(!readOnly);
            this->m_spinBox->resize(option.rect.size());
            int min = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Min).data().toInt();
            int max = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Max).data().toInt();
            if (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>() == QTOZW_ValueIds::ValueIdTypes::Int) {
                max = qMin(max, std::numeric_limits<int>::max());
            } else if (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>() == QTOZW_ValueIds::ValueIdTypes::Byte) {
                max = qMin(static_cast<uint8_t>(max), std::numeric_limits<uint8_t>::max());
            } else if (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>() == QTOZW_ValueIds::ValueIdTypes::Short) {
                max = qMin(static_cast<uint16_t>(max), std::numeric_limits<uint16_t>::max());
            }
            this->m_spinBox->setRange(min, max);
            painter->save();
            painter->translate(option.rect.topLeft());
            this->m_spinBox->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);
            painter->restore();
            break;
        }
        default: {
            return QStyledItemDelegate::paint(painter, option, index);
        }

    }
}

QSize QTOZW_ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QModelIndex typeIndex = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Type);
    switch (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>()) {
        case QTOZW_ValueIds::ValueIdTypes::List: {
            return this->m_comboBox->minimumSizeHint();
        }
        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            return this->m_checkBox->minimumSizeHint();
        }
        case QTOZW_ValueIds::ValueIdTypes::Int:
        case QTOZW_ValueIds::ValueIdTypes::Byte:
        case QTOZW_ValueIds::ValueIdTypes::Short: {
            return this->m_spinBox->minimumSize();
        }
        default:
            return QStyledItemDelegate::sizeHint(option, index);
    }
}
