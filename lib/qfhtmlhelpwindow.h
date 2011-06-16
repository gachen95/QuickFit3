#ifndef QFHTMLHelpWindow_H
#define QFHTMLHelpWindow_H

#include <QTextBrowser>
#include <QLabel>
#include <QFont>
#include <QString>
#include <QSettings>
#include <QStringList>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStack>
#include <QUrl>

/*! \brief this is a window that displays information from a given HTML file
    \ingroup qf3lib_widgets
 */

class QFHTMLHelpWindow : public QWidget {
        Q_OBJECT

    public:
        /** \brief class constructor */
        QFHTMLHelpWindow(QWidget* parent=0);

        /** \brief read widget size and position from a QSettings object where prefix is prepended to all keys */
        void readSettings(QSettings& settings, QString prefix=QString(""));

        /** \brief write widget size and position to a QSettings object where prefix is prepended to all keys */
        void writeSettings(QSettings& settings, QString prefix=QString(""));


        /** \brief return a reference to the font of the title line */
        QFont& titleFont() { return m_titleFont; }

        /** \brief return a reference to the base font of the main text */
        QFont& baseFont() { return m_baseFont; }

    public slots:
        /** \brief updates the information in the window */
        void updateHelp(QString title, QString filename);
        /** \brief updates the information in the window. The title is taken from the HTML page */
        void updateHelp(QString filename);
        /** \brief clear contents of window */
        void clear();
    private:
        /** \brief a label for the model name */
        QLabel* labelTitle;

        QPushButton* btnPrevious;
        QPushButton* btnNext;
        QPushButton* btnHome;
        QPushButton* btnPrint;


        /** \brief a text edit for the model information/description */
        QTextBrowser* descriptionBrowser;

        /** \brief current searchPath for assets */
        QString searchPath;

        /** \brief font of the title line */
        QFont m_titleFont;
        /** \brief base font of the main text */
        QFont m_baseFont;

        QString m_home;

        QStack<QString> history;

        int history_idx;

    private slots:
        void displayTitle();
        void anchorClicked(const QUrl& link);
        void previous();
        void next();
        void home();
        void showFile(QString filename);
        void print();
};

#endif // QFHTMLHelpWindow_H



