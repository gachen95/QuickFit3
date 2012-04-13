#ifndef QF3TMCLPROTOCOLHANDLER_H
#define QF3TMCLPROTOCOLHANDLER_H
#include "jkserialconnection.h"
#include <QString>
#include "qfpluginservices.h"


//Opcodes of all TMCL commands that can be used in direct mode
#define TMCL_ROR 1
#define TMCL_ROL 2
#define TMCL_MST 3
#define TMCL_MVP 4
#define TMCL_SAP 5
#define TMCL_GAP 6
#define TMCL_STAP 7
#define TMCL_RSAP 8
#define TMCL_SGP 9
#define TMCL_GGP 10
#define TMCL_STGP 11
#define TMCL_RSGP 12
#define TMCL_RFS 13
#define TMCL_SIO 14
#define TMCL_GIO 15
#define TMCL_SCO 30
#define TMCL_GCO 31
#define TMCL_CCO 32

//Opcodes of TMCL control functions (to be used to run or abort a TMCL program in the module)
#define TMCL_APPL_STOP 128
#define TMCL_APPL_RUN 129
#define TMCL_APPL_RESET 131
#define TMCL_GETFIRMWARE 136

//Options for MVP commandds
#define MVP_ABS 0
#define MVP_REL 1
#define MVP_COORD 2

//Options for RFS command
#define RFS_START 0
#define RFS_STOP 1
#define RFS_STATUS 2

//Result codes for GetResult
#define TMCL_RESULT_OK 0
#define TMCL_RESULT_NOT_READY 1
#define TMCL_RESULT_CHECKSUM_ERROR 2

/*! \brief tool class to handle the TMCL protocol
    \ingroup qf3extensionplugins

    see:
      - <a href="http://www.trinamic.com/tmc/media/Downloads/PANdrives/PD-108-28/PD-108-28_TMCL_firmware_manual.pdf">PDx-108-28-SE TMCL (tm) Firmware Manual</a>
      - <a href="http://www.trinamic.com/tmc/media/Downloads/modules/TMCL/AppNotes/TMCLDatagram.c">http://www.trinamic.com/tmc/media/Downloads/modules/TMCL/AppNotes/TMCLDatagram.c</a>
      - <a href="http://www.google.de/url?sa=t&rct=j&q=trinamic%20tmcl&source=web&cd=1&ved=0CDoQFjAA&url=http%3A%2F%2Fwww.produktinfo.conrad.com%2Fdatenblaetter%2F175000-199999%2F198696-an-01-en-TMCL_Reference_PANDRIVE_42mm_RS232_0_50_NM.pdf&ei=vP-GT6XAN7TP4QTr-fWmBw&usg=AFQjCNFVSMqY0sjFagev3r_8riksmTAjRg&cad=rja">TMCL (tm) Reference and Programming Manual</a>
    .
*/
class QF3TMCLProtocolHandler {

    public:


        /** \brief class constructor
         *
         *  \param com com port to use
         *  \param name name of the device (for error messages)
         */
        QF3TMCLProtocolHandler(JKSerialConnection* com, QString name);

        void setLogging(QFPluginLogService* log, QString LOG_PREFIX);

        bool checkComConnected();


        /** \brief send a TMCL command, receive the answer */
        bool sendAndCheckCommand(uint8_t Address, uint8_t Command, uint8_t Type, uint8_t Motor, int32_t Value);
        /** \brief send a TMCL command, receive the answer and return address, status and value from the answer  */
        bool queryCommand(uint8_t Address, uint8_t Command, uint8_t Type, uint8_t Motor, int32_t Value, uint8_t *rAddress, uint8_t *rStatus, int32_t *rValue);
        /** \brief send a TMCL command, receive the answer and return the status from the answer  */
        bool queryStatus(uint8_t Address, uint8_t Command, uint8_t Type, uint8_t Motor, int32_t Value, uint8_t *rStatus);

        bool setAxisParameter(uint8_t Address, uint8_t parameter, int32_t value, uint8_t motor=0);
        bool getAxisParameter(uint8_t Address, uint8_t parameter, int32_t& value, uint8_t motor=0);
        bool setGlobalParameter(uint8_t Address, uint8_t parameter, int32_t value, uint8_t bank=0);
        bool getGlobalParameter(uint8_t Address, uint8_t parameter, int32_t& value, uint8_t bank=0);
        QString getFirmwareVersion(uint8_t Address, uint8_t motor);

        void checkComError();

        bool hasErrorOccured();
        QString getLastError();
    protected:
        /** \brief send a TMCL command
         *
         *  \note You will have to call readResult() after this command to receive/swallow the answer .... otherwise it will go on residing in the COM buffer!
         */
        bool sendCommand(uint8_t Address, uint8_t Command, uint8_t Type, uint8_t Motor, int32_t Value);
        /** \brief read an answer */
        int readResult(uint8_t *Address, uint8_t *Status, int32_t *Value);
        /** \brief read the answer of a command 136 (get firmware version) */
        bool readVersionResult(QString &version);

        JKSerialConnection* com;
        QFPluginLogService* log;
        QString LOG_PREFIX;
        QString name;
        /** \brief number of retries, when sending a command */
        int retries;
        /** \brief milliseconds to wait, before receiving an answer */
        int command_delay_ms;
};

#endif // QF3TMCLPROTOCOLHANDLER_H