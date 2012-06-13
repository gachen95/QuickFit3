#ifndef QFFitFunctionsSPIMFCSDiffE2E2_H
#define QFFitFunctionsSPIMFCSDiffE2E2_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a SPIM-FCS fit model with pure diffusion and lateral 1/e^2 width
    \ingroup qf3fitfunp_fitfunctions_spimfcs

    \f[ g(\tau)=G_\infty+\frac{1}{N}\cdot\frac{1}{a^2}\cdot\sum\limits_{i=1}^3\rho_i\cdot\left[a\cdot\sqrt{\pi}\cdot\mbox{erf}\left(\frac{a}{\sqrt{4D_i\tau+\sigma_{xy}^2}}\right)+\sqrt{4D_i\tau+\sigma_{xy}^2}\cdot\left[\exp\left(-\frac{a^2}{4D_i\tau+\sigma_{xy}^2}\right)-1\right]\right]^2\cdot\left[1+\frac{4D_i\tau}{\sigma_z^2}\right]^{-1/2} \f]

    \f[ V_{\text{eff}}=\pi^{3/2}\cdot a^2\cdot \sigma_z \f]
    \f[ \Delta V_{\text{eff}}=\sqrt{\left(\Delta\sigma_z\cdot \pi^{3/2}a^2\right)^2+\left(\Delta a\cdot 2\pi^{3/2}a\sigma_z\right)^2} \f]
    \f[ C=\frac{N}{V_{\text{eff}}}=\frac{N}{\pi^{3/2}\cdot a^2\cdot \sigma_z} \f]
    \f[ \Delta C=\sqrt{\left(\frac{\Delta N}{\pi^{3/2}a^2\cdot\sigma_z}\right)^2+\left(\frac{\Delta\sigma_z\cdot N}{\pi^{3/2}a^2\sigma_z^2}\right)^2+\left(\frac{\Delta a\cdot 2\cdot N}{\pi^{3/2}a^3\cdot\sigma_z}\right)^2} \f]
*/
class QFFitFunctionsSPIMFCSDiffE2: public QFFitFunction {
    public:
        QFFitFunctionsSPIMFCSDiffE2();
        virtual ~QFFitFunctionsSPIMFCSDiffE2() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("SPIM-FCS: Diffusion (1/e^2 radii)"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("SPIM-FCS: Diffusion (1/e^2 radii)"); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("fcs_spim_diffe2"); }

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

        /*! \copydoc QFFitFunction::get_implementsDerivatives()   */
        virtual bool get_implementsDerivatives() { return false; }

        /*! \copydoc QFFitFunction::evaluateDerivatives()   */
        virtual void evaluateDerivatives(double* derivatives, double t, const double* parameters) const ;

        /*! \copydoc QFFitFunction::sortParameter()   */
        virtual void sortParameter(double* parameterValues, double* error=NULL) const;
};

#endif // QFFitFunctionsSPIMFCSDiffE2E2_H