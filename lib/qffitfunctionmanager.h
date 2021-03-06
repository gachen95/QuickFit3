/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFFITFUNCTIONMANAGER_H
#define QFFITFUNCTIONMANAGER_H

#include "lib_imexport.h"
#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
#include "qffitfunction.h"
#include "qfpluginfitfunction.h"
#include "programoptions.h"
#include "qfpluginservices.h"

/*! \brief this class manages the available fitting functions
    \ingroup qf3lib_fitting

    As not ever model (fitting function) is applicable to any problem, this class provides
    means to sleect only a sub-group of parameters. They are selected by the start of their
    ID string, so getModels("fcs_") will return all models that start with "fcs_".
*/
class QFLIB_EXPORT QFFitFunctionManager : public QObject {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFitFunctionManager(ProgramOptions* options, QObject* parent);
        /** Default destructor */
        virtual ~QFFitFunctionManager();


        /** \brief return a list of all implemented IDs */
        QStringList getIDList(int i) const ;
        /** \brief return a list of all implemented IDs */
        QStringList getIDList() const ;

        bool contains(const QString& ID) const;

        /** \brief return number of available plugins */
        inline int pluginCount() const { return fitPlugins.size(); }

        QObject* getPluginObject(int i) const;

        /** \brief name for the plugin */
        virtual QString getName(int i) const;

        /** \brief plugin filename */
        virtual QString getPluginFilename(int i) const;

        /** \brief short description for the plugin */
        virtual QString getDescription(int i) const;

        /** \brief plugin filename */
        virtual QString getIconFilename(int i) const;

        /** \brief author the plugin */
        virtual QString getAuthor(int i) const;

        /** \brief ID of the plugin */
        virtual QString getID(int i) const;

        /** \brief copyright information the plugin */
        virtual QString getCopyright(int i) const;

        /** \brief weblink for the plugin */
        virtual QString getWeblink(int i) const;

        /** \brief returns the plugins main help file (html) for a specified  ID. */
        QString getPluginHelp(int ID) const;
        /** \brief returns the plugins tutorial file (html) for a specified  ID. */
        QString getPluginTutorialMain(int ID) const;
        /** \brief returns the plugins FAQ file (html) for a specified  ID. */
        QString getPluginFAQ(int ID) const;

        /** \brief returns lists of titles (names) and links to all tutorials for for a specified plugin ID. */
        void getPluginTutorials(int ID, QStringList& names, QStringList& files) const;
        /** \brief returns the plugins settings file (html) for a specified  ID. */
        QString getPluginSettings(int ID) const;
        /** \brief returns the plugins copyright file (html) for a specified  ID. */
        QString getPluginCopyrightFile(int ID) const;
        /** \brief returns a fit algorthms \a faID main help file (html) for a specified plugin \a ID. */
        QString getPluginHelp(int ID, QString faID) const;

        /** \brief return a list of the models which start with the specified string
         *         all instance are newly created with the given parent in every call!
         *
         *  \note This function is thread-safe
         */
        QMap<QString, QFFitFunction*> getModels(QString id_start=QString("")) const;

        static void freeModels(QMap<QString, QFFitFunction *> &models);
        /** \brief return a list of the models which start with the specified string
         *
         *  \note This function is thread-safe
         */
        QStringList getModelIDs(QString id_start=QString("")) const;
        /** \brief returns a new instance of the model which is specified by ID
         *
         *         the instance are newly created with the given parent in every call!
         *
         *  \note This function is thread-safe
*/
        QFFitFunction* createFunction(const QString &ID) const;

        /** \brief return the plugin index for a given fit function id */
        int getPluginForID(QString id) const;

        inline bool hasPluginForID(const QString& id) const {
            return getPluginForID(id)>=0;
        }

        /** \brief get plugins major version number */
        int getMajorVersion(int i) const;
        /** \brief get plugins minor version number */
        int getMinorVersion(int i) const;

        /** \brief returns a pointer to the instance of the main manager object (singleton within a QuickFit3 instance) */
        static QFFitFunctionManager* getInstance() {
            if (QFPluginServices::getInstance()) {
                return QFPluginServices::getInstance()->getFitFunctionManager();
            }
            return NULL;
        }

        QStringList getUserFitFunctionIDs() const;
        QString getUserFitFunctionFile(const QString& id) const;

        QStringList getLibraryFitFunctionIDs() const;
        QLibrary* getLibraryFitFunctionLibrary(const QString& id) const;

        void registerFitFactory(QFFitFunctionFactory* factory);
        void unregisterFitFactory(QFFitFunctionFactory* factory);

        void deinit();
        void init();
        bool registerPlugin(const QString &filename_in, QObject *plugin, QFPluginHelpData &helpdata);
        void finalizePluginSearch();

        static QStringList getUserFitFunctionSearchDirectories(bool for_libs=false);
    public slots:
        void reloadUserFitFunctions();

    protected:
        void reloadLibraryFitFunctions();
        void freeLibraryFitFunctions(bool emitSignals=true);
    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);

        void fitFunctionsChanged();

        void incProgress();
    private:
        QMap<QString, QLibrary*> libraryFitFunctions;
        QMap<QString, QString> userFitFunctions;
        QList<QFPluginFitFunction*> fitPlugins;
        QList<QFFitFunctionFactory*> fitFactories;
        QList<QObject*> fitPluginObjects;
        QStringList filenames;
        ProgramOptions* m_options;

        QMutex* mutex;
};

#endif // QFFITFUNCTIONMANAGER_H
