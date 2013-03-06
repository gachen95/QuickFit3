#include "qfplotter.h"
#include "qfplotterprivate.h"


QFPlotter::QFPlotter(QWidget *parent) :
    JKQtPlotter(parent)
{
    p=new QFPlotterPrivate(this);
}

QFPlotter::QFPlotter(bool datastore_internal, QWidget *parent, JKQTPdatastore *datast):
    JKQtPlotter(datastore_internal, parent, datast)
{
    p=new QFPlotterPrivate(this);
}