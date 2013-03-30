#ifndef QFIMFCCSPARAMETERINPUTTABLE_H
#define QFIMFCCSPARAMETERINPUTTABLE_H

#include <QAbstractTableModel>
#include "qfrawdatarecord.h"
class QFImFCCSFitEvaluationItem; //forward

class QFImFCCSParameterInputTable : public QAbstractTableModel
{
        Q_OBJECT
    public:
        enum QFImFCCSParameterInputTableWidgetTypes {
            wtNone=0,
            wtRDRCombobox=1,
            wtFitFunctionCombobox=2,
            wtFixCheck=3,
            wtValueDoubleEdit=4,
            wtValueComboBox=5,
            wtValueIntEdit=6,
            wtErrorEdit=7
        };

        enum QFImFCCSParameterInputTableRoles {
            rdrIDRole= Qt::UserRole,
            fitFunctionIDRole= Qt::UserRole+1,
            widgetTypeRole=  Qt::UserRole+2,
            fitParameterIDRole= Qt::UserRole+3,
            evaluationIDRole= Qt::UserRole+4,
            fitFileIDRole=Qt::UserRole+4
        };

        explicit QFImFCCSParameterInputTable(QFImFCCSFitEvaluationItem *parent = 0);
        

        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

        int getColsPerRDR() const;
    signals:
        void modelRebuilt();
    public slots:
        void rebuildModel();
    public:
        bool checkRebuildModel(bool alwaysreset=false);
    protected:
        QFImFCCSFitEvaluationItem* item;

        int colsPerRDR;
        struct FitParam {
            FitParam();
            bool isFit;
            QString id;
            QString label;
            QString tooltip;
            QSet<QFRawDataRecord*> visibleIn;
            bool isValid() const;

            bool operator==(const FitParam& other)const;
        };

        QList<FitParam> fitparamids;

        static bool fitParamListContainsID(const QString& id, const QList<FitParam>& fitparamids, int* found=NULL);
        
};

#endif // QFIMFCCSPARAMETERINPUTTABLE_H