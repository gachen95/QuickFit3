#include "qfextensioncamera.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

QFExtensionCameraImplementation::QFExtensionCameraImplementation(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFExtensionCameraImplementation::~QFExtensionCameraImplementation() {

}


void QFExtensionCameraImplementation::deinit() {
	/* add code for cleanup here */
}

void QFExtensionCameraImplementation::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionCameraImplementation::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
}

void QFExtensionCameraImplementation::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(QApplication::applicationDirPath()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionCameraImplementation::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(QApplication::applicationDirPath()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

	}

unsigned int QFExtensionCameraImplementation::getCameraCount() {
    return 1;
	/* how man cameras may be accessed by your plugin (e.g. if you use one driver to access several cameras */
}
void QFExtensionCameraImplementation::useCameraSettings(unsigned int camera, const QSettings& settings) {
    /* set the camera settings to the values specified in settings parameter */
}

void QFExtensionCameraImplementation::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/


	/////////////////////////////////////////////////////////////////////////////////
	// if you want the settings dialog to be modal, you may uncomment the next lines
	// and add implementations
	/////////////////////////////////////////////////////////////////////////////////
    /*
	QDialog* dlg=new QDialog(parent);

    QVBoxLayout* lay=new QVBoxLayout(dlg);
    dlg->setLayout(lay);

    QFormLayout* formlayout=new QFormLayout(dlg);


    //  create your widgets here, do not to initialize them with the current settings
    // QWidget* widget=new QWidget(dlg);
    // lay->addRow(tr("Name"), widget);
    // lay->setValue(settings.value(QString("device/name%1").arg(camera), devfaultValue ).toInt());


    lay->addLayout(formlayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    if ( dlg->exec()==QDialog::Accepted ) {
         //  read back values entered into the widgets and store in settings
         // settings.setValue(QString("device/name%1").arg(camera), widget->value() );
    }
    delete dlg;
	*/
}

int QFExtensionCameraImplementation::getImageWidth(unsigned int camera) {
    return /* width of the next image from the given camera */;
}

int QFExtensionCameraImplementation::getImageHeight(unsigned int camera) {
    return /* height of the next image from the given camera */;
}

bool QFExtensionCameraImplementation::isConnected(unsigned int camera) {
    return /* are we connected to the specified camera */;
}

bool QFExtensionCameraImplementation::acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp) {
    if (timestamp!=NULL) {
        *timestamp=/* store a timestamp information */;
    }

	/*
	    acquire a new image from the given camera and store it to data
		You may expect that data is at least as large as getImageWidth(camera)*getImageHeight(camera)
	*/
}

bool QFExtensionCameraImplementation::connectDevice(unsigned int camera) {
    /* connect to the given camera */
	return true;
}

void QFExtensionCameraImplementation::disconnectDevice(unsigned int camera) {
    /* disconnect from the given camera */
}

double QFExtensionCameraImplementation::getExposureTime(unsigned int camera) {
    return /* exposure time of the last image */;
}

bool QFExtensionCameraImplementation::startAcquisition(unsigned int camera, QString filenamePrefix) {
    return false;
}

bool QFExtensionCameraImplementation::cancelAcquisition(unsigned int camera) {
    return true;
}

bool QFExtensionCameraImplementation::isAcquisitionRunning(unsigned int camera, double* percentageDone) {
    return false;
}

void QFExtensionCameraImplementation::getAcquisitionDescription(unsigned int camera, QStringList* files, QMap<QString, QVariant>* parameters) {
}

bool QFExtensionCameraImplementation::getAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

void QFExtensionCameraImplementation::log_indent() {
	if (logService) logService->log_indent();
	else if (services) services->log_indent();
}

void QFExtensionCameraImplementation::log_unindent() {
	if (logService) logService->log_unindent();
	else if (services) services->log_unindent();
}

void QFExtensionCameraImplementation::log_text(QString message) {
	if (logService) logService->log_text(message);
	else if (services) services->log_text(message);
}

void QFExtensionCameraImplementation::log_warning(QString message) {
	if (logService) logService->log_warning(message);
	else if (services) services->log_warning(message);
}

void QFExtensionCameraImplementation::log_error(QString message) {
	if (logService) logService->log_error(message);
	else if (services) services->log_error(message);
}


Q_EXPORT_PLUGIN2(TARGETNAME, QFExtensionCameraImplementation)
