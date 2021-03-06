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

#ifndef QFFitFunctionCSFCSDiffD_H
#define QFFitFunctionCSFCSDiffD_H
#include "qfpluginfitfunction.h"
#include "qffitfunction.h"
#include "qfgeneralfitfunctionbase.h"


/*! \brief plugin class for circle scanning FCS Normal Diffusion Fit Functions
    \ingroup qf3fitfunp_fcs


*/
class QFFitFunctionCSFCSDiffD: public QFFCSFitFunctionBase {
    public:
    QFFitFunctionCSFCSDiffD(bool hasGamma=true);
    virtual ~QFFitFunctionCSFCSDiffD() {}

        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const;
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const;
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const { return QObject::tr("Confocal FCS"); }

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double x, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params) const;

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) const;

    protected:
        bool hasGamma;

};

#endif // QFFitFunctionCSFCSDiffD_H
