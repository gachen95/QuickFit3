#include "controlWidget.h"
#include "ui_controlWidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include "qfpluginservices.h"
#include "programoptions.h"

controlWidget::controlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::controlWidget)
{
    ui->setupUi(this);
    this->setWindowTitle("ALEX Control");
//    this->setWindowIcon(QIcon("sequence.svg"));

    QIcon icon("AlexControlIcon.png");
    this->setWindowIcon(icon);

    ui->groupBox_file->hide();
    connect(ui->pushButtonExternalSDFFfile,SIGNAL(clicked()),this,SLOT(setExternalSDFF()));
    connect(ui->checkBoxSlow,SIGNAL(toggled(bool)),this,SLOT(setSlowAlternation(bool)));
    connect(this,SIGNAL(fileChanged(QString)),ui->lineEdit,SLOT(setText(QString)));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(setSDFFfile(QString)));
    connect(this,SIGNAL(fileChanged(QString)),this,SLOT(setSDFFfile(QString)));
    connect(ui->pushButtonSave,SIGNAL(clicked()),this,SLOT(saveSDFF()));
    connect(ui->comboBoxSequence,SIGNAL(currentIndexChanged(int)),this,SLOT(setSequence(int)));
    ui->comboBoxSequence->setDisabled(true);

    cmanager = new SDFFLib::channelManager;


    loadSettings(*(ProgramOptions::getInstance()->getQSettings()));

    ni_driver=NULL;
    checkIniFiles();

}

controlWidget::~controlWidget()
{
    delete ui;
}

void controlWidget::saveSettings(QSettings &settings, QString prefix)
{
    settings.setValue(prefix+"spinBoxTimestep",ui->spinBoxTimestep->value());
    settings.setValue(prefix+"spinBox_T_D",ui->spinBox_1->value());
    settings.setValue(prefix+"spinBox_Tbreak1",ui->spinBox_2->value());
    settings.setValue(prefix+"spinBox_T_A",ui->spinBox_3->value());
    settings.setValue(prefix+"spinBox_Tbreak2",ui->spinBox_4->value());
    settings.setValue(prefix+"doubleSpinBoxIntensityBlue",ui->doubleSpinBoxIntensityBlue->value());
    settings.setValue(prefix+"doubleSpinBoxIntensityGreen",ui->doubleSpinBoxIntensityGreen->value());
    settings.setValue(prefix+"comboBoxSequence",ui->comboBoxSequence->currentIndex());
    settings.setValue(prefix+"lineEdit",ui->lineEdit->text());

    settings.setValue(prefix+"geometry",this->saveGeometry());
    settings.setValue(prefix+"checkBoxOptions",ui->checkBoxOptions->isChecked());
    settings.setValue(prefix+"channelsFilename",channelsFilename);
    settings.setValue(prefix+"portFilename",portFilename);
    settings.setValue(prefix+"timingFilename",timingFilename);
    settings.setValue(prefix+"curFile",curFile);
    settings.setValue(prefix+"directory",directory);
    settings.setValue(prefix+"sdffSequence",sdffSequence);


}

void controlWidget::loadSettings(QSettings &settings, QString prefix)
{
    ui->spinBoxTimestep->setValue(settings.value(prefix+"spinBoxTimestep",10).toInt());
    ui->spinBox_1->setValue(settings.value("spinBox_T_D",100).toInt());
    ui->spinBox_2->setValue(settings.value("spinBox_Tbreak1",10).toInt());
    ui->spinBox_3->setValue(settings.value("spinBox_T_A",100).toInt());
    ui->spinBox_4->setValue(settings.value("spinBox_Tbreak2",10).toInt());
    ui->comboBoxSequence->setCurrentIndex(settings.value("comboBoxSequence",0).toInt());
    ui->doubleSpinBoxIntensityGreen->setValue(settings.value("doubleSpinBoxIntensityGreen",10).toDouble());
    ui->doubleSpinBoxIntensityBlue->setValue(settings.value("doubleSpinBoxIntensityBlue",10).toDouble());
    ui->lineEdit->setText(settings.value("lineEdit","").toString());

    this->restoreGeometry(settings.value("geometry",this->geometry()).toByteArray());
    ui->checkBoxOptions->setChecked(settings.value("checkBoxOptions",false).toBool());
    channelsFilename = settings.value("channelsFilename",QFPluginServices::getInstance()->getGlobalConfigFileDirectory()+"/alexc_channels.ini").toString();
    portFilename = settings.value("portFilename",QFPluginServices::getInstance()->getGlobalConfigFileDirectory()+"/alexc_ports.ini").toString();
    timingFilename = settings.value("timingFilename",QFPluginServices::getInstance()->getGlobalConfigFileDirectory()+"/alexc_timing.ini").toString();
    curFile = settings.value("curFile",QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_alexcontrol")+"/ALEX_sequence.sdff").toString();
    directory = settings.value("directory",QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_alexcontrol")).toString();
    sdffSequence = settings.value("sdffSequence",QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_alexcontrol")+"/ALEX_sequence.sdff").toString();
}

void controlWidget::closeEvent(QCloseEvent *event)
{
    //QSettings settings(QApplication::applicationDirPath()+"/" FILENAME_SETTINGS,QSettings::IniFormat);
    this->saveSettings(*(ProgramOptions::getInstance()->getQSettings()));
}

void controlWidget::openFileDialog()
{

    QString filename =QFileDialog::getOpenFileName(this, tr("Open SDFF File"),
                                               directory, tr("SDFF Files (*.sdff);;All Files (*.*)"));
    emit fileChanged(filename);
    //qDebug("filename = %s", filename.toStdString().c_str());

}

void controlWidget::setSDFFfile(QString filename)
{
    qDebug()<<"setSDFFfile filename = "<<filename;
    curFile=filename;
}

void controlWidget::setSequence(int index)
{
    switch(index) {
    case 0:
        curFile=QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_alexcontrol")+"/ALEX_sequence.sdff"; break;
    case 1:
        curFile=QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_alexcontrol")+"/ALEX_sequence_slow.sdff"; break;
    case 2:
        curFile=QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_alexcontrol")+"/BlueOnly.sdff"; break;
    case 3:
        curFile=QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_alexcontrol")+"/GreenOnly.sdff"; break;
    case 4:
        curFile=QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_alexcontrol")+"/BlueGreen.sdff"; break;
    case 5:
        curFile=ui->lineEdit->text(); break;
    default:
        qWarning("unknown sequence");
    }
//    if(index>0) {
//        ui->spinBox_1->setDisabled(true);
//        ui->spinBox_2->setDisabled(true);
//        ui->spinBox_3->setDisabled(true);
//        ui->spinBox_4->setDisabled(true);
//        ui->checkBoxSlow->setDisabled(true);
//    } else {
//        ui->spinBox_1->setDisabled(false);
//        ui->spinBox_2->setDisabled(false);
//        ui->spinBox_3->setDisabled(false);
//        ui->spinBox_4->setDisabled(false);
//        ui->checkBoxSlow->setDisabled(false);
//    }
    qDebug()<<"setSequence = "<<curFile;
}

void controlWidget::setExternalSDFF()
{
    ui->comboBoxSequence->setCurrentIndex(5);
    runSDFFOnHardware();
}

void controlWidget::setSlowAlternation(bool toggle)
{
    if(toggle) {
        ui->spinBox_1->setSuffix(tr("ms"));
        ui->spinBox_2->setSuffix(tr("ms"));
        ui->spinBox_3->setSuffix(tr("ms"));
        ui->spinBox_4->setSuffix(tr("ms"));
        ui->spinBox_1->setValue(1000);
        ui->spinBox_2->setValue(200);
        ui->spinBox_3->setValue(1000);
        ui->spinBox_4->setValue(200);
    } else {
        ui->spinBox_1->setSuffix(tr("�s"));
        ui->spinBox_2->setSuffix(tr("�s"));
        ui->spinBox_3->setSuffix(tr("�s"));
        ui->spinBox_4->setSuffix(tr("�s"));
        ui->spinBox_1->setValue(100);
        ui->spinBox_2->setValue(20);
        ui->spinBox_3->setValue(100);
        ui->spinBox_4->setValue(20);
    }
}


void controlWidget::connectToHardware() {

    qDebug("connect to hardware");

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (ni_driver) delete ni_driver;
    bool ok=true;

    qDebug("try: ");
    try {
        cmanager->read_ini(std::string(channelsFilename.toLocal8Bit().data()));
        ni_driver=new SDFFLib::NIdriver(cmanager);
        ni_driver->set_timing_inifile(timingFilename.toStdString());   // load alexc_timing.ini (default: no variable timing)

    } catch (SDFFLib::exception& E) {
        SDFFLib::bi_event ev;
        ev.type=SDFFLib::biError;
        ev.position=E.get_line();
        ev.time=E.get_time();
        ev.text=E.get_message();
        ev.stext=ev.text;
        ev.num=E.get_number();
        ev.filename="";
        errorModel->addEvent(ev);
        ok=false;
    } catch (std::exception& E) {
        SDFFLib::bi_event ev;
        ev.type=SDFFLib::biError;
        ev.position=-1;
        ev.time=-1;
        ev.text=std::string("internal C++ stdlib error:\n")+E.what();
        ev.stext=ev.text;
        ev.num=-1;
        ev.filename="";
        errorModel->addEvent(ev);
        ok=false;
    }
    qDebug("success");

    QApplication::restoreOverrideCursor();
}



void controlWidget::runSDFFOnHardware() {

    if (!ni_driver) {
        connectToHardware();
    }
    if (!ni_driver) {
        QMessageBox::critical(NULL, tr("SDFF Player"), tr("Cannot run sequence on hardware:\nNI DAQmx driver not connected!"));
        return;
    }
    sdffParser=NULL;
    qDebug("try: ");
    try {

        QApplication::setOverrideCursor(Qt::WaitCursor);
        cmanager->read_ini(std::string(channelsFilename.toLocal8Bit().data()));
        qDebug("successfully read channels.ini");
        double timestep=ui->spinBoxTimestep->value(); // in us

        SDFFLib::NIsequenceGenerator* nisg=new SDFFLib::NIsequenceGenerator();
        nisg->set_portconfigfilename(portFilename.toStdString());
        nisg->init(cmanager, timestep);
        nisg->set_use_variable_timing(false);

        ni_driver->set_continuoustask(true);
        sdffParser = new SDFFLib::SDFFparser(nisg, cmanager);
        qDebug("NIsequenceGenerator initialized.\nloading file..");
        if(!QFile(curFile).exists()) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this,"Warning","The file \"" + curFile + "\" does not exist");
            return;
        }
        std::string fn=curFile.replace('/', "\\").toLocal8Bit().data();

        int r=sdffParser->import(fn);
        if(r) {
//            qDebug(sdffParser->get_report().c_str());
            QMessageBox msgImportError(this);
            msgImportError.setIcon(QMessageBox::Warning);
            msgImportError.setWindowTitle("Warning");
            msgImportError.addButton(QMessageBox::Close);
            msgImportError.setEscapeButton(QMessageBox::Close);
            msgImportError.setText("Import error.");
            msgImportError.setDetailedText(QString::fromStdString(sdffParser->get_report()));
            int ret = msgImportError.exec();
            return;
        };


        if(ui->comboBoxSequence->currentIndex()==5);
        else {
            QString params;
            double conversionFactor=1e-6;
            if(ui->checkBoxSlow->isChecked()) conversionFactor=1e-3;
            else conversionFactor=1e-6;
            params =   "t1="       + QString::number((double)ui->spinBox_1->value()*conversionFactor) + // convert units according to conversionFactor to seconds
                       ";\nt2="    + QString::number((double)ui->spinBox_2->value()*conversionFactor) +
                       ";\nt3="    + QString::number((double)ui->spinBox_3->value()*conversionFactor) +
                       ";\nt4="    + QString::number((double)ui->spinBox_4->value()*conversionFactor) +
                       ";\nblue="  + QString::number(ui->doubleSpinBoxIntensityBlue->value()) +
                       ";\ngreen=" + QString::number(ui->doubleSpinBoxIntensityGreen->value()) +
                       ";";
            if(ui->comboBoxSequence->currentIndex()>0)
                params= "blue="  + QString::number(ui->doubleSpinBoxIntensityBlue->value()) +
                        ";\ngreen=" + QString::number(ui->doubleSpinBoxIntensityGreen->value()) +
                        ";";
            qDebug() << params;
            SDFFLib::sdff_result *res=new SDFFLib::sdff_result();
            sdffParser->evaluate_parameters(params.toStdString(), res);
            delete res;

            // qDebug(SDFFLib::sdff_resulttostr(*res).c_str());
//            qDebug(sdffParser->report_contents().c_str());
//            qDebug(sdffParser->get_cmd_manager()->report().c_str());
            sdffSequence=sdffParser->get_current_sdff_code().c_str();
            qDebug(sdffParser->get_current_sdff_code().c_str());
        }

        qDebug("running SDFF on hardware ... generate ...");

        r=sdffParser->generate(timestep);

        qDebug("Bind...");

        ni_driver->bind(nisg);

        qDebug("Start...");

        ni_driver->start();

        QModernProgressDialog* prg=new QModernProgressDialog(tr("running SDFF sequence on hardware ..."), tr("Cancel"), this);
        prg->setHasCancel(true);
        prg->setMode(true, false);
        prg->setWindowModality(Qt::WindowModal);
        prg->show();

        QTime t=QTime::currentTime();
        while (!ni_driver->is_done()) {
            int el=t.elapsed();
            ulong min=el/60000;
            ulong sec=(el%60000)/1000;
            prg->setLabelText(tr("running SDFF sequence on hardware ...\n   runtime: %1:%2").arg(min,2,10,QChar('0')).arg(sec,2,10,QChar('0')));
            QApplication::processEvents();
            if (prg->wasCanceled()) {
                QApplication::processEvents();
                ni_driver->stop();
                break;
            }
        }
        QApplication::processEvents();
        ni_driver->clear();
        delete prg;

        delete nisg;

    } catch (SDFFLib::exception& E) {
        SDFFLib::bi_event ev;
        ev.type=SDFFLib::biError;
        ev.position=E.get_line();
        ev.time=E.get_time();
        ev.text=E.get_message();
        ev.stext=ev.text;
        ev.num=E.get_number();
        ev.filename="";
        errorModel->addEvent(ev);
    } catch (std::exception& E) {
        SDFFLib::bi_event ev;
        ev.type=SDFFLib::biError;
        ev.position=-1;
        ev.time=-1;
        ev.text=std::string("internal C++ stdlib error:\n")+E.what();
        ev.stext=ev.text;
        ev.num=-1;
        ev.filename="";
        errorModel->addEvent(ev);
    }
    if(sdffParser!=NULL) delete sdffParser;
    sdffParser=NULL;

    QApplication::restoreOverrideCursor();

}


void controlWidget::checkIniFiles()
{
    // check if all required ini files are there

    bool ok=true;
    QMessageBox msgIniMissing(this);
    msgIniMissing.setIcon(QMessageBox::Warning);
    msgIniMissing.setWindowTitle("Warning");
    msgIniMissing.addButton(QMessageBox::Open);
    msgIniMissing.setDefaultButton(QMessageBox::Open);
    msgIniMissing.addButton(QMessageBox::Close);
    msgIniMissing.setEscapeButton(QMessageBox::Close);
    QString explanation=tr("\n\nThis file configures the NI hardware and defines which IOs to use for what. Examples for this file are available in the directory:\n   %1").arg(ProgramOptions::getInstance()->getApplicationDirectory()+"/globalconfig_templates");
    while(ok && !QFile(channelsFilename).exists()) {
        msgIniMissing.setText(tr("The file alexc_channels.ini is missing. %1\n\nLooking for file: %2").arg(explanation).arg(channelsFilename));
        int ret = msgIniMissing.exec() ;
        if(QMessageBox::Open==ret) {
            channelsFilename=QFileDialog::getOpenFileName(this, tr("Open Channel Settings"),
                                                          directory, tr("ini Files (*.ini);;All Files (*.*)"));
        } else {
            close();
            ok=false;
            break;
        }
    }
    while(ok && !QFile(timingFilename).exists()) {
        msgIniMissing.setText(tr("The file alexc_timing.ini is missing. %1\n\nLooking for file: %2").arg(explanation).arg(timingFilename));
        int ret = msgIniMissing.exec() ;
        if(QMessageBox::Open==ret) {
            timingFilename=QFileDialog::getOpenFileName(this, tr("Open Timing Settings"),
                                                          directory, tr("ini Files (*.ini);;All Files (*.*)"));
        } else {
            close();
            ok=false;
            break;
        }
    }
    while(ok && !QFile(portFilename).exists()) {
        msgIniMissing.setText(tr("The file alexc_ports.ini is missing. %1\n\nLooking for file: %2").arg(explanation).arg(portFilename));
        int ret = msgIniMissing.exec() ;
        if(QMessageBox::Open==ret) {
            portFilename=QFileDialog::getOpenFileName(this, tr("Open Port Settings"),
                                                          directory, tr("ini Files (*.ini);;All Files (*.*)"));
        } else {
            close();
            ok=false;
            break;
        }
    }

    if (!ok) {
        close();
        deleteLater();
    }
}


void controlWidget::on_doubleSpinBoxIntensityBlue_editingFinished() // only executed when pressed "enter"
{
    if(!ui->doubleSpinBoxIntensityBlue->hasFocus()) return;
    else runSDFFOnHardware();
}

void controlWidget::on_doubleSpinBoxIntensityGreen_editingFinished()// only executed when pressed "enter"
{
    if(!ui->doubleSpinBoxIntensityGreen->hasFocus()) return;
    else runSDFFOnHardware();
}

void controlWidget::on_comboBoxSequence_editingFinished()
{
    if(!ui->comboBoxSequence->hasFocus()) return;
    else runSDFFOnHardware();
}

void controlWidget::on_pushButtonPlay_clicked()
{
    if(ui->checkBoxSlow->isChecked()) ui->comboBoxSequence->setCurrentIndex(1);
    else ui->comboBoxSequence->setCurrentIndex(0);
    runSDFFOnHardware();
}

void controlWidget::on_pushButtonD_clicked()
{
    ui->comboBoxSequence->setCurrentIndex(2);
    runSDFFOnHardware();
}

void controlWidget::on_pushButtonA_clicked()
{
    ui->comboBoxSequence->setCurrentIndex(3);
    runSDFFOnHardware();
}

void controlWidget::on_pushButtonDA_clicked()
{
    ui->comboBoxSequence->setCurrentIndex(4);
    runSDFFOnHardware();
}

void controlWidget::on_pushButtonLoadDefault_clicked()
{
    QSettings settings;
    loadSettings(settings);
}

void controlWidget::on_lineEdit_returnPressed()
{
    ui->comboBoxSequence->setCurrentIndex(externalSDFF);
    runSDFFOnHardware();
}


void controlWidget::saveSDFF() // FIXME
{
    qDebug("saveSDFF clicked");

    QString filename =  QFileDialog::getSaveFileName(this, tr("Save File"),
                        directory, tr("SDFF files (*.sdff);;Text files (*.asc *.txt)"
                        ";;All Files (*.*)"));

    qDebug()<<filename;
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {qWarning()<<"could not open file"<<filename; return;}
    QTextStream out(&file);
    out<<"test\n";
    out <<"channels: "          <<  channelsFilename
        <<"\nports: "           <<  portFilename
        <<"\ntiming: "          <<  timingFilename
        <<"\ncurrent file: "    << curFile
        <<"\ncurrent directory:"<< directory
       <<"\n\n";
    out << sdffSequence;
    file.close();
}

void controlWidget::fileDlg_fileSelected(const QString &)
{
    qDebug("File changed");
}

