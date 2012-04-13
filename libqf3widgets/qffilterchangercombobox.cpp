#include "qffilterchangercombobox.h"

QFFilterChangerComboBox::QFFilterChangerComboBox(QFExtensionManager* extManager, QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=extManager;
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    findExtensions();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}

QFFilterChangerComboBox::QFFilterChangerComboBox(QWidget* parent):
    QEnhancedComboBox(parent)
{
    m_extManager=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
}
QFFilterChangerComboBox::~QFFilterChangerComboBox()
{
    //dtor
}

void QFFilterChangerComboBox::init(QFExtensionManager* extManager) {
    m_extManager=extManager;
    findExtensions();
}

void QFFilterChangerComboBox::findExtensions() {
    FilterChangers.clear();
    clear();
    if (m_extManager==NULL) return;
    // load available acquisition devices from extManager
    QStringList ids=m_extManager->getIDList();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=m_extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=m_extManager->getInstance(ids[i]);
        QFExtensionFilterChanger* FilterChanger = qobject_cast<QFExtensionFilterChanger*>(extobj);
        if (FilterChanger) FilterChangers.append(extobj);
    }
    QPoint sl(-1,-1);
    addItem(QIcon(":/libqf3widgets/filterchanger_none.png"), tr("--- none ---"), sl);

    for (int i=0; i<FilterChangers.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(FilterChangers[i]);
        QFExtensionFilterChanger* FilterChanger = qobject_cast<QFExtensionFilterChanger*>(FilterChangers[i]);
        for (unsigned int j=0; j<FilterChanger->getFilterChangerCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            QString cname=FilterChanger->getFilterChangerShortName(j);
            if (FilterChanger->getFilterChangerCount()>0) name=name+QString(" #%1").arg(j).arg((cname!=name && !cname.isEmpty())?QString(" (%1)").arg(cname):QString(""));
            addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }

}

QFExtension* QFFilterChangerComboBox::currentExtension() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionFilterChanger* FilterChanger=NULL;
    //int FilterChangerIdx=p.y();
    if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
        extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
        FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
    }
    return extension;
}

QFExtensionFilterChanger* QFFilterChangerComboBox::currentExtensionFilterChanger() const {
    if (currentIndex()<0) {
        return NULL;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionFilterChanger* FilterChanger=NULL;
    //int FilterChangerIdx=p.y();
    if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
        extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
        FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
    }
    return FilterChanger;
}

int QFFilterChangerComboBox::currentFilterChangerID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    QFExtension* extension=NULL;
    QFExtensionFilterChanger* FilterChanger=NULL;
    int FilterChangerIdx=p.y();
    if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
        extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
        FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
    } else {
        FilterChangerIdx=-1;
    }
    return FilterChangerIdx;
}

int QFFilterChangerComboBox::currentExtensionID() const {
    if (currentIndex()<0) {
        return -1;
    }

    QPoint p = itemData(currentIndex()).toPoint();
    return p.x();
}

void QFFilterChangerComboBox::comboIndexChanged(int index) {

    QPoint p = itemData(index).toPoint();
    QFExtension* extension=NULL;
    QFExtensionFilterChanger* FilterChanger=NULL;
    int FilterChangerIdx=p.y();
    if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
        extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
        FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
    } else {
        FilterChangerIdx=-1;
    }
}

void QFFilterChangerComboBox::storeSettings(QSettings& settings, QString prefix) const {
    if (currentExtension()) {
        settings.setValue(prefix+"filterchanger_plugin_id", currentExtension()->getID());
        settings.setValue(prefix+"filterchanger_id", currentFilterChangerID());
    } else {
        settings.setValue(prefix+"filterchanger_plugin_id", "");
        settings.setValue(prefix+"filterchanger_id", -1);
    }
}

void QFFilterChangerComboBox::loadSettings(QSettings& settings, QString prefix) {
   QString id=settings.value(prefix+"filterchanger_plugin_id", "").toString();
   int FilterChangerIdx=settings.value(prefix+"filterchanger_id", -1).toInt();
   setCurrentIndex(0);
   bool ok=false;
   QFExtension* extension=NULL;
   QFExtensionFilterChanger* FilterChanger=NULL;
   for (int i=0; i<count(); i++) {
       QPoint p = itemData(i).toPoint();
       int FilterChangerIdxC=p.y();
       if ((p.x()>=0)&&(p.x()<FilterChangers.size())) {
           extension=qobject_cast<QFExtension*>(FilterChangers[p.x()]);
           FilterChanger=qobject_cast<QFExtensionFilterChanger*>(FilterChangers[p.x()]);
           if (extension && extension->getID()==id && FilterChangerIdx==FilterChangerIdxC) {
               ok=true;
               setCurrentIndex(i);
               break;
           }
       }
   }
   if (!ok) setCurrentIndex(0);

}