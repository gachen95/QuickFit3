#ifndef IMFCSCALIBRATIONWIZARD_H
#define IMFCSCALIBRATIONWIZARD_H

#include <QDialog>

namespace Ui {
    class ImFCSCalibrationWizard;
}

class ImFCSCalibrationWizard : public QDialog
{
        Q_OBJECT
        
    public:
        explicit ImFCSCalibrationWizard(QWidget *parent = 0);
        ~ImFCSCalibrationWizard();

    signals:
        void run1();
        void run2();
        void run3();
        void run4();
        void run5();
        void showTutorial();
    protected slots:
        void on_btnHelp_clicked();
        void on_btnClose_clicked();
        void on_btnStep1_clicked();
        void on_btnStep2_clicked();
        void on_btnStep3_clicked();
        void on_btnStep4_clicked();
        void on_btnStep5_clicked();

    private:
        Ui::ImFCSCalibrationWizard *ui;
};

#endif // IMFCSCALIBRATIONWIZARD_H