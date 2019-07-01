#include <QCheckBox>
#include <QDebug>

#include "bitsetwidget.h"
#include "ui_bitsetwidget.h"

BitSetWidget::BitSetWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BitSetWidget)
{
    ui->setupUi(this);
}

BitSetWidget::~BitSetWidget()
{
    delete ui;
}

void BitSetWidget::setValue(QTOZW_ValueIDBitSet value) {
    this->m_value = value;
    for (uint8_t i = 0; i <= this->m_value.values.size() -1; ++i) {
        if (this->m_value.mask.at(i)) {
            QCheckBox *cb = new QCheckBox(this);
            cb->setText(this->m_value.label[i]);
            cb->setChecked(this->m_value.values.at(i));
            cb->setToolTip(this->m_value.help[i]);
            cb->setProperty("BitSetIndex", i);
            QObject::connect(cb, &QCheckBox::stateChanged, this, &BitSetWidget::cbChanged);
            this->ui->gridLayout_2->addWidget(cb);
        }
    }
}

void BitSetWidget::cbChanged() {
    QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
    if (!cb)
        throw std::logic_error("Widget is not of Type QCheckBox");
    int index = cb->property("BitSetIndex").toInt();
    if (this->m_value.values.at(index) != cb->isChecked())
        this->m_value.values[index] = cb->isChecked();
    emit stateChanged();
}

QTOZW_ValueIDBitSet BitSetWidget::getValue() {
    return this->m_value;

}
