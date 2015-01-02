#ifndef QFGROUPANDROLEMODEL_H
#define QFGROUPANDROLEMODEL_H

#include <QAbstractTableModel>
#include "qfproject.h"
#include <QStyledItemDelegate>

class QFGroupAndRoleModel : public QAbstractTableModel
{
        Q_OBJECT
    public:
        explicit QFGroupAndRoleModel(QFProject* project, QObject *parent = 0);

        virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
        virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    signals:

    public slots:
        void setRoleAlwaysEditable(bool enabled);
        void setNameEditable(bool enabled);
    protected slots:
        void projectDataChanged();
    protected:
        QFProject* project;
        bool roleAlwaysEditable;
        bool nameEditable;

};



class QFGroupAndRoleDelegate : public QStyledItemDelegate {
        Q_OBJECT
    public:
        QFGroupAndRoleDelegate(QFProject* project, QObject* parent=NULL);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    private:
    protected slots:
        void currentIndexChanged(int index);
    protected:
        QFProject* project;
};

#endif // QFGROUPANDROLEMODEL_H