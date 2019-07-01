#ifndef QTOZW_ITEMDELEGATE_H
#define QTOZW_ITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>

class QTOZW_ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QTOZW_ItemDelegate(QObject *parent = nullptr);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;


private slots:
    void commitAndCloseEditor();
};

#endif // QTOZW_ITEMDELEGATE_H
