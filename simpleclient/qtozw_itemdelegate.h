#ifndef QTOZW_ITEMDELEGATE_H
#define QTOZW_ITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>

class QTOZW_ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QTOZW_ItemDelegate(QObject *parent = nullptr);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;


signals:

public slots:

private:
    QComboBox* m_comboBox;
};

#endif // QTOZW_ITEMDELEGATE_H
