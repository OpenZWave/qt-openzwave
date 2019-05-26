#include <QPainter>
#include <QDebug>
#include <QApplication>
#include "qtozw_itemdelegate.h"
#include "qtozwvalueidmodel.h"

QTOZW_ItemDelegate::QTOZW_ItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QString BitSettoQString(QBitArray ba) {
    QString result;
    for (int i = 0; i < ba.size(); ++i) {
        if (ba.testBit(i))
            result[i] = '1';
        else
            result[i] = '0';
    }
    result.prepend("0b");
    return result;
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
            QStyleOptionComboBox comboBoxOption;
            comboBoxOption.rect = option.rect;
            comboBoxOption.state = option.state;
            if (readOnly) {
                comboBoxOption.state |= QStyle::State_ReadOnly;
            }
            comboBoxOption.currentText = val.selectedItem;

            QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &comboBoxOption, painter);
            QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &comboBoxOption, painter);

            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            QStyleOptionButton cbOption;
            cbOption.rect = option.rect;
            cbOption.state |= index.data().value<bool>() ? QStyle::State_On : QStyle::State_Off;
            cbOption.state |= QStyle::State_Enabled;
            if (readOnly)
                cbOption.state |= QStyle::State_ReadOnly;
            QApplication::style()->drawControl(QStyle::CE_CheckBox, &cbOption, painter);
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::BitSet: {
            QStyleOptionViewItem itemOption(option);
            initStyleOption(&itemOption, index);
            itemOption.text = BitSettoQString(index.data().value<QTOZW_ValueIDBitSet>().values);
            qDebug() << itemOption.text;
            QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter);
        }
#if 0
        case QTOZW_ValueIds::ValueIdTypes::Int:
        case QTOZW_ValueIds::ValueIdTypes::Byte:
        case QTOZW_ValueIds::ValueIdTypes::Short: {
            QSpinBox sb;
            sb.setValue(index.data().toInt());
            sb.resize(option.rect.size());
            this->m_spinBox->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);


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
#endif
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
            QTOZW_ValueIDList val = index.data().value<QTOZW_ValueIDList>();
            QStyleOptionComboBox comboBoxOption;
            comboBoxOption.rect = option.rect;
            comboBoxOption.state = option.state;
            comboBoxOption.currentText = val.selectedItem;
            return QSize(QFontMetrics(option.font).width(val.selectedItem), comboBoxOption.rect.height());
        }
        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            QStyleOptionButton cbOption;
            cbOption.rect = option.rect;
            cbOption.state |= index.data().value<bool>() ? QStyle::State_On : QStyle::State_Off;
            cbOption.state |= QStyle::State_Enabled;
            return cbOption.rect.size();
        }
        default:
            return QStyledItemDelegate::sizeHint(option, index);
    }
}

QWidget *QTOZW_ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QModelIndex typeIndex = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Type);
    switch (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>()) {
        case QTOZW_ValueIds::ValueIdTypes::List: {
            QComboBox *editor = new QComboBox(parent);
            editor->setAutoFillBackground(true);
            connect(editor, &QComboBox::currentTextChanged, this, &QTOZW_ItemDelegate::commitAndCloseEditor);
            return editor;
        }
        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            QCheckBox *editor = new QCheckBox(parent);
            editor->setAutoFillBackground(true);
            return editor;
        }
        case QTOZW_ValueIds::ValueIdTypes::Int:
        case QTOZW_ValueIds::ValueIdTypes::Short:
        case QTOZW_ValueIds::ValueIdTypes::Byte: {
            QSpinBox *editor = new QSpinBox(parent);
            editor->setAutoFillBackground(true);
            return editor;
        }

        default:
            return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void QTOZW_ItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

void QTOZW_ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QModelIndex typeIndex = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Type);
    switch (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>()) {
        case QTOZW_ValueIds::ValueIdTypes::List: {
            QComboBox *cb =  static_cast<QComboBox*>(editor);
            QTOZW_ValueIDList val = index.data().value<QTOZW_ValueIDList>();
            cb->addItems(val.labels);
            cb->setCurrentText(val.selectedItem);
            cb->showPopup();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            QCheckBox *cb = static_cast<QCheckBox*>(editor);
            bool value = index.data().toBool();
            cb->setChecked(value);
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Int:
        case QTOZW_ValueIds::ValueIdTypes::Short:
        case QTOZW_ValueIds::ValueIdTypes::Byte: {
            QSpinBox *sb = qobject_cast<QSpinBox *>(editor);
            int min = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Min).data().toInt();
            int max = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Max).data().toInt();
            if (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>() == QTOZW_ValueIds::ValueIdTypes::Int) {
                max = qMin(max, std::numeric_limits<int>::max());
            } else if (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>() == QTOZW_ValueIds::ValueIdTypes::Byte) {
                max = qMin(static_cast<uint8_t>(max), std::numeric_limits<uint8_t>::max());
            } else if (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>() == QTOZW_ValueIds::ValueIdTypes::Short) {
                max = qMin(static_cast<uint16_t>(max), std::numeric_limits<uint16_t>::max());
            }
            sb->setRange(min, max);
            sb->setValue(index.data().toInt());
            break;
        }

        default:
            QStyledItemDelegate::setEditorData(editor, index);
    }
}

void QTOZW_ItemDelegate::commitAndCloseEditor() {
    QWidget *editor = qobject_cast<QWidget *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

void QTOZW_ItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QModelIndex typeIndex = index.sibling(index.row(), QTOZW_ValueIds::ValueIdColumns::Type);
    switch (typeIndex.data().value<QTOZW_ValueIds::ValueIdTypes>()) {
        case QTOZW_ValueIds::ValueIdTypes::List: {
            QComboBox* cb = qobject_cast<QComboBox*>(editor);
            if (!cb)
                throw std::logic_error("editor is not a combo box");
            QTOZW_ValueIDList val = index.data().value<QTOZW_ValueIDList>();
            if (val.selectedItem != cb->currentText()) {
                val.selectedItem = cb->currentText();
                model->setData(index, QVariant::fromValue<QTOZW_ValueIDList>(val), Qt::EditRole);
            }
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            QCheckBox* cb = qobject_cast<QCheckBox *>(editor);
            if (!cb)
                throw std::logic_error("editor is not a checkbox");
            bool value = index.data().toBool();
            if (value != cb->isChecked()) {
                model->setData(index, cb->isChecked(), Qt::EditRole);
            }
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Int:
        case QTOZW_ValueIds::ValueIdTypes::Short:
        case QTOZW_ValueIds::ValueIdTypes::Byte: {
            QSpinBox* sb = qobject_cast<QSpinBox *>(editor);
            if (!sb)
                throw std::logic_error("Editor is not a spinbox");
            int value = index.data().toInt();
            if (value != sb->value()) {
                model->setData(index, sb->value(), Qt::EditRole);
            }
            break;
        }

        default:
            break;
    }
}
