#ifndef BITSETWIDGET_H
#define BITSETWIDGET_H

#include <QFrame>

namespace Ui {
class BitSetWidget;
}

class BitSetWidget : public QFrame
{
    Q_OBJECT

public:
    explicit BitSetWidget(QWidget *parent = nullptr);
    ~BitSetWidget();

private:
    Ui::BitSetWidget *ui;
};

#endif // BITSETWIDGET_H
