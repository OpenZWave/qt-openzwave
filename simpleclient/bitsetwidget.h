#ifndef BITSETWIDGET_H
#define BITSETWIDGET_H

#include <QFrame>
#include <qtozwvalueidmodel.h>

namespace Ui {
class BitSetWidget;
}

class BitSetWidget : public QFrame
{
    Q_OBJECT

public:
    explicit BitSetWidget(QWidget *parent = nullptr);
    ~BitSetWidget();
    void setValue(QTOZW_ValueIDBitSet);
    QTOZW_ValueIDBitSet getValue();

Q_SIGNALS:
    void stateChanged();

private Q_SLOTS:
    void cbChanged();

private:
    Ui::BitSetWidget *ui;
    QTOZW_ValueIDBitSet m_value;
};

#endif // BITSETWIDGET_H
