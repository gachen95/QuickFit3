#ifndef HEADER_H
#define HEADER_H

#include "qfpluginimporters.h"
#include "importer_inst_f1.h"

/*!
    \defgroup doxygen_GROUPNAME Importer Plugin
    \ingroup qf3importerplugins
*/



/*! \brief plugin class for Importers
    \ingroup doxygen_GROUPNAME
*/
class QFFitAlgorithmInst : public QObject, public QFPluginImporters {
        Q_OBJECT
        Q_INTERFACES(QFPluginImporters)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFFitAlgorithmInst(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFFitAlgorithmInst() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "target_id";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/importer_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFImporter* createImporter(QString id) const ;
		
		QF_PLUGIN
    private:
};

#endif // HEADER_H