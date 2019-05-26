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
