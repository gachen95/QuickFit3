/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qffitresultsbyindexevaluationfittools.h"
#include "qffitresultsbyindexevaluation.h"


QFFitStatistics QFFitResultsByIndexEvaluationFitToolsBase::calcFitStatistics(bool storeAsResults, QFFitFunction *ffunc, long N, const double *tauvals, const double *corrdata, const double *weights, int datacut_min, int datacut_max, const double *fullParams, const double *errors, const bool *paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord *record, int run, const QString& prefix, const QString& pgroup, const QVector<double> &COV, double paramrange_size, bool storeCOV, QStringList *returnFitParamNames)
{
    QFFitStatistics result= ffunc->calcFitStatistics(N, tauvals, corrdata, weights, datacut_min, datacut_max, fullParams, errors, paramsFix, runAvgWidth, residualHistogramBins, COV, paramrange_size, storeCOV, returnFitParamNames);

    if (storeAsResults) {
        saveFitStatistics(result, record, run, prefix, pgroup);
    }

    return result;
}

void QFFitResultsByIndexEvaluationFitToolsBase::saveFitStatistics(QFBasicFitStatistics &results, QFRawDataRecord *record, int run, const QString &prefix, const QString &pgroup)
{
    QFFitResultsByIndexEvaluation* t=dynamic_cast<QFFitResultsByIndexEvaluation*>(this);
    if (record && t) {
        QString eid= t->getEvaluationResultID(run, record);
        t->setFitResultBasicFitStatistics(record, eid, results, prefix, pgroup);
    }
}



QFFitResultsByIndexEvaluationFitToolsBase::QFFitResultsByIndexEvaluationFitToolsBase()
{
    mutexThreadedFit=new QMutex(QMutex::Recursive);
}

QFFitResultsByIndexEvaluationFitToolsBase::~QFFitResultsByIndexEvaluationFitToolsBase()
{
    delete mutexThreadedFit;
}
