/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:40:44 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3508 $)

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

#ifndef QFFitFunctionGeneralPDFCauchy_H
#define QFFitFunctionGeneralPDFCauchy_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunctions_general

*/
class QFFitFunctionGeneralPDFCauchy: public QFFitFunction {
    public:
        QFFitFunctionGeneralPDFCauchy();
        virtual ~QFFitFunctionGeneralPDFCauchy() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QObject::tr("Probability Density (PDF): Cauchy function"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("PDF: Cauchy"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("gen_pdf_cauchy"); };
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const { return QObject::tr("General Fit Functions/Probability Density (PDF)"); }

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params);

        bool estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix=NULL);
};

#endif // QFFitFunctionGeneralPDFCauchy_H
