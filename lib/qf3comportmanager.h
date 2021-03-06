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

#ifndef QF3COMPORTMANAGER_H
#define QF3COMPORTMANAGER_H
#include <QVector>
#include <QList>
#include <QString>
#include <QSettings>
#include <QMutex>
#include "qfserialconnection.h"
#include "lib_imexport.h"

/*! \brief you may use this class to manage a list of COM ports.
    \ingroup qf3extensionplugins

    The function addCOMPort() reads a port configuration from QSettings. If the given port is already in the list,
    its ID is returned, otherwise the port is added.
 */

class QFLIB_EXPORT QF3ComPortManager {
    public:
        QF3ComPortManager();
        ~QF3ComPortManager();

        void clear();

        int addCOMPort(QSettings& settings, QString prefix=QString(""), int defaultSpeed=9600, QFSCdatabits databits=QFSC8databits,  QFSCparity parity=QFSCnoParity, QFSChandshaking handshaking=QFSCnoHandshaking, QFSCstopbits stopbits=QFSConeStopbit, int defaulttimeout=500);
        void storeCOMPort(int port, QSettings& settings, QString prefix=QString(""));

        QFSerialConnection* getCOMPort(int port) const;
        QMutex* getMutex(int port) const;
    protected:
        struct COMPORTS {
            /** \brief serial connection object */
            QFSerialConnection* com;
            QMutex* mutex;
        };
        QList<COMPORTS> coms;
};

#endif // QF3COMPORTMANAGER_H
