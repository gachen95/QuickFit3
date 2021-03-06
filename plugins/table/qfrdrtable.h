/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFRDRTABLE_H
#define QFRDRTABLE_H

#include "qftablepluginmodel.h"

#include "qfrawdatarecord.h"
#include "qfrawdatarecordfactory.h"
#include "qfplotter.h"
#include "jkqtptools.h"
#include "qftools.h"
#include "qfrdrtableinterface.h"
#include "qfrdrcolumngraphsinterface.h"
#include "qfmathparser.h"
#include <QPointer>
#include <QMultiMap>
#include "datatools.h"
#include "qffitalgorithmparameterstorage.h"
#include "qfrdrcurvesinterface.h"
class QFRDRTableEditor; // forward

/*! \brief this class is used to manage a table of values (strings/numbers)
    \ingroup qf3rdrdp_table

    The data is stored in a QFTablePluginModel object which is also externally accessible for data access.
 */

class QFRDRTable: public QFRawDataRecord, public QFRDRTableInterface, public QFRDRColumnGraphsInterface, public QFFitAlgorithmParameterStorage, public QFRDRUserCurvesInterface {
        Q_OBJECT
        Q_INTERFACES(QFRDRTableInterface QFRDRColumnGraphsInterface QFRDRUserCurvesInterface)
    public:
        enum {
            TableExpressionRole = Qt::UserRole+1,
            TableCommentRole = Qt::UserRole+2,
            ColumnExpressionRole = Qt::UserRole+10,
            ColumnCommentRole = Qt::UserRole+11,
            ColumnImagePixWidth = Qt::UserRole+12,
            ColumnImagePixHeight = Qt::UserRole+13,
            ColumnImageWidth = Qt::UserRole+14,
            ColumnImageHeight = Qt::UserRole+15,
            ColumnImageX = Qt::UserRole+16,
            ColumnImageY = Qt::UserRole+17
        };


        enum GraphType {
            gtLines=0,
            gtImpulsesVertical,
            gtImpulsesHorizontal,
            gtFilledCurveX,
            gtFilledCurveY,
            gtStepsHorizontal,
            gtStepsVertical,
            gtBarsHorizontal,
            gtBarsVertical,
            gtBoxplotX,
            gtBoxplotY,
            gtImage,
            gtRGBImage,
            gtMaskImage,
            gtFunction,
            gtHorizontalRange,
            gtVerticalRange,
            gtParametrizedScatter
        };

        enum GTFunctionType {
            gtfString=0,
            gtfPolynomial=1,
            gtfExponential=2,
            gtfPowerLaw=3,
            gtfQFFunction=4
        };

        static QString GTFunctionType2String(GTFunctionType type) {
            switch(type) {
                case gtfQFFunction: return QString("qffunction");
                case gtfPolynomial: return QString("poly");
                case gtfExponential: return QString("exp");
                case gtfPowerLaw: return QString("pow");
                default:
                case gtfString:
                    return QString("string");
            }

        }
        static GTFunctionType String2GTFunctionType(const QString& type) {
            if(type=="qffunction") return gtfQFFunction;
            if(type=="poly") return gtfPolynomial;
            if(type=="exp") return gtfExponential;
            if(type=="pow") return gtfPowerLaw;
            return gtfString;
        }

        static QString GraphType2String(GraphType type) {
            switch(type) {
                case gtLines: return QString("lines");
                case gtImpulsesVertical: return QString("impulsesv");
                case gtImpulsesHorizontal: return QString("impulsesh");
                case gtFilledCurveX: return QString("filledx");
                case gtFilledCurveY: return QString("filledy");
                case gtStepsHorizontal: return QString("stepsh");
                case gtStepsVertical: return QString("stepsv");
                case gtBarsHorizontal: return QString("barsh");
                case gtBarsVertical: return QString("barsv");
                case gtBoxplotX: return QString("boxx");
                case gtBoxplotY: return QString("boxy");
                case gtImage: return QString("image");
                case gtRGBImage: return QString("rgbimage");
                case gtMaskImage: return QString("maskimage");
                case gtFunction: return QString("function");
                case gtHorizontalRange: return QString("hrange");
                case gtVerticalRange: return QString("vrange");
                case gtParametrizedScatter: return QString("pscatter");
                default: return QString("");
            }
            return QString("");
        }

        static QIcon GraphType2Icon(GraphType type) {
            switch(type) {
                case gtLines: return QIcon(":/table/icons/plot_pointslines.png");
                case gtImpulsesVertical: return QIcon(":/table/icons/plot_vimpulses.png");
                case gtImpulsesHorizontal: return QIcon(":/table/icons/plot_himpulses.png");
                case gtFilledCurveX: return QIcon(":/table/icons/plot_xfilledcurve.png");
                case gtFilledCurveY: return QIcon(":/table/icons/plot_yfilledcurve.png");
                case gtStepsHorizontal: return QIcon(":/table/icons/plot_hsteps.png");
                case gtStepsVertical: return QIcon(":/table/icons/plot_vsteps.png");
                case gtBarsHorizontal: return QIcon(":/table/icons/plot_hbars.png");
                case gtBarsVertical: return QIcon(":/table/icons/plot_vbars.png");
                case gtBoxplotX: return QIcon(":/table/icons/plot_boxplotsx.png");
                case gtBoxplotY: return QIcon(":/table/icons/plot_boxplotsy.png");
                case gtImage: return QIcon(":/table/icons/plot_image.png");
                case gtRGBImage: return QIcon(":/table/icons/plot_rgbimage.png");
                case gtMaskImage: return QIcon(":/table/icons/plot_maskimage.png");
                case gtFunction: return QIcon(":/table/icons/plot_function.png");
                case gtHorizontalRange: return QIcon(":/table/icons/plot_hrange.png");
                case gtVerticalRange: return QIcon(":/table/icons/plot_vrange.png");
                case gtParametrizedScatter: return QIcon(":/table/icons/plot_pscatter.png");
                default: return QIcon();
            }
            return QIcon();
        }





        static GraphType String2GraphType(const QString& type) {
            QString s=type.trimmed().toLower();
            if (s=="lines") return gtLines;
            if (s=="impulsesv") return gtImpulsesVertical;
            if (s=="impulsesh") return gtImpulsesHorizontal;
            if (s=="filledx") return gtFilledCurveX;
            if (s=="filledy") return gtFilledCurveY;
            if (s=="stepsh") return gtStepsHorizontal;
            if (s=="stepsv") return gtStepsVertical;
            if (s=="barsh") return gtBarsHorizontal;
            if (s=="barsv") return gtBarsVertical;
            if (s=="boxx") return gtBoxplotX;
            if (s=="boxy") return gtBoxplotY;
            if (s=="image") return gtImage;
            if (s=="rgbimage") return gtRGBImage;
            if (s=="maskimage") return gtMaskImage;
            if (s=="function") return gtFunction;
            if (s=="vrange") return gtVerticalRange;
            if (s=="hrange") return gtHorizontalRange;
            if (s=="pscatter") return gtParametrizedScatter;
            return gtLines;
        }


        enum DataSelectOperation {
            dsoEquals=0,
            dsoUnequal=1,
            dsoGreaterOrEqual=2,
            dsoSmallerOrEqual=3,
            dsoGreater=4,
            dsoSmaller=5,
            dsoInRange=6,
            dsoOutOfRange=7,
        };
        enum DataSelectLogicOperation {
            dsoNone=0,
            dsoAnd=1,
            dsoOr=2,
            dsoXor=3
        };

        static QString DataSelectOperation2String(DataSelectOperation type) {
            switch(type) {
                case dsoUnequal: return QString("!=");
                case dsoGreaterOrEqual: return QString(">=");
                case dsoSmallerOrEqual: return QString("<=");
                case dsoGreater: return QString(">");
                case dsoSmaller: return QString("<");
                case dsoInRange: return QString("inrange");
                case dsoOutOfRange: return QString("outrange");
                default:
                case dsoEquals: return QString("==");
            }

        }
        static DataSelectOperation String2DataSelectOperation(const QString& type) {
            if(type=="!=") return dsoUnequal;
            if(type==">=") return dsoGreaterOrEqual;
            if(type=="<=") return dsoSmallerOrEqual;
            if(type==">") return dsoGreater;
            if(type=="<") return dsoSmaller;
            if(type=="inrange") return dsoInRange;
            if(type=="outrange") return dsoOutOfRange;
            return dsoEquals;
        }

        static QString DataSelectLogicOperation2String(DataSelectLogicOperation type) {
            switch(type) {
                case dsoAnd: return QString("AND");
                case dsoOr: return QString("OR");
                case dsoXor: return QString("XOR");
                default:
                case dsoNone: return QString("NONE");
            }

        }
        static DataSelectLogicOperation String2DataSelectLogicOperation(const QString& type) {
            if(type.toUpper()=="AND") return dsoAnd;
            if(type.toUpper()=="OR") return dsoOr;
            if(type.toUpper()=="XOR") return dsoXor;
            if(type.toUpper()=="NONE") return dsoNone;
            return dsoNone;
        }


        enum RangeGraphMode {
            rgmRange=0,
            rgmLineError=1,
            rgmLineOnly=2
        };


        static QString RangeGraphMode2String(RangeGraphMode type) {
            switch(type) {
                case rgmLineError: return QString("LINEERR");
                case rgmLineOnly: return QString("LINE");
                default:
                case rgmRange: return QString("RANGE");
            }

        }
        static RangeGraphMode String2RangeGraphMode(const QString& type) {
            if(type.toUpper()=="LINEERR") return rgmLineError;
            if(type.toUpper()=="LINE") return rgmLineOnly;
            if(type.toUpper()=="RANGE") return rgmRange;
            return rgmRange;
        }

        struct GraphInfo {
            GraphInfo();
            //void setBoxplotColumns(int position, int minC, int q25C, int medianC, int meanC, int q75C, int maxC);
            GraphType type;
            QString title;
            bool titleShow;
            int xcolumn;
            int ycolumn;
            int xerrorcolumn;
            int yerrorcolumn;
            int meancolumn;
            int q75column;
            int maxcolumn;
            double width;
            double shift;
            Qt::PenStyle style;
            Qt::PenStyle whiskerStyle;
            Qt::BrushStyle fillStyle;
            QColor color;
            double colorTransparent;
            QColor errorColor;
            QColor fillColor;
            double errorColorTransparent;
            double fillColorTransparent;
            double linewidth;
            JKQTPgraphSymbols symbol;
            double symbolSize;
            double symbolLineWidth;
            JKQTPerrorPlotstyle errorStyle;
            bool drawLine;

            double errorWidth;
            Qt::PenStyle errorLineStyle;
            double errorBarSize;

            double offset;
            int xerrorcolumnlower;
            int yerrorcolumnlower;
            bool xerrorsymmetric;
            bool yerrorsymmetric;

            int dataSortOrder;



            int stride;
            int strideStart;
            bool isStrided;

            bool isDataSelect;
            int dataSelect1Column;
            DataSelectOperation dataSelect1Operation;
            double dataSelect1CompareValue;
            double dataSelect1CompareValue2;

            DataSelectLogicOperation dataSelectLogic12;
            int dataSelect2Column;
            DataSelectOperation dataSelect2Operation;
            double dataSelect2CompareValue;
            double dataSelect2CompareValue2;

            DataSelectLogicOperation dataSelectLogic23;
            int dataSelect3Column;
            DataSelectOperation dataSelect3Operation;
            double dataSelect3CompareValue;
            double dataSelect3CompareValue2;


            int imageTicks;
            int imageModTicks;
            double imageColorbarFontsize;
            double imageColorbarTicklength;

            JKQTPCAlabelType imageColorbarLabelType;
            int imageColorbarLabelDigits;

            QColor imageTrueColor;
            double imageTrueTransparent;
            QColor imageFalseColor;
            double imageFalseTransparent;
            int imagePixelWidth;
            double imageX;
            double imageY;
            double imageWidth;
            double imageHeight;
            JKQTPMathImageColorPalette imagePalette;
            double imageMin;
            double imageMax;
            bool imageAutoRange;
            QString imageLegend;
            QString imageLegendG;
            QString imageLegendB;
            QString imageLegendMod;
            bool imageColorbarRight;
            bool imageColorbarTop;
            int colorbarWidth;
            double colorbarRelativeHeight;
            JKQTPMathImage::ModifierMode modifierMode;

            QString function;
            int subfunction;
            bool showallsubfunctions;
            GTFunctionType functionType;
            QVector<double> functionParameters;

            double rangeStart;
            double rangeEnd;
            double rangeCenter;
            bool rangeInverted;
            bool rangeFill;
            QColor rangeCenterColor;
            double rangeCenterColorTransparent;
            Qt::PenStyle rangeCenterStyle;
            double rangeCenterWidth;
            bool rangeDrawCenter;
            RangeGraphMode rangeMode;

            bool errorColorAuto;
            bool fillColorAuto;
            bool centerColorAuto;

            QMap<QString, QVariant> moreProperties;

            QFRDRCurvesInterface::CurveType getCurvesCurveType() const;
            bool isCurvesCurve() const;


        };

        struct GraphDataSelection {
            GraphDataSelection();
            GraphDataSelection(const GraphDataSelection& g);
            GraphDataSelection& operator=(const GraphInfo& g);
            GraphDataSelection& operator=(const GraphDataSelection& g);
            int stride;
            int strideStart;
            bool isStrided;

            bool isDataSelect;
            int dataSelect1Column;
            DataSelectOperation dataSelect1Operation;
            double dataSelect1CompareValue;
            double dataSelect1CompareValue2;

            DataSelectLogicOperation dataSelectLogic12;
            int dataSelect2Column;
            DataSelectOperation dataSelect2Operation;
            double dataSelect2CompareValue;
            double dataSelect2CompareValue2;

            DataSelectLogicOperation dataSelectLogic23;
            int dataSelect3Column;
            DataSelectOperation dataSelect3Operation;
            double dataSelect3CompareValue;
            double dataSelect3CompareValue2;


            bool getDataWithStride(QVector<double> *dataOut, int* colOut, int column, const QFRDRTable* table, QString* newname=NULL, QFPlotter *plotter=NULL);
        };

        struct AxisInfo {
            AxisInfo();
            QString label;
            double min, max;
            bool log;
            QColor axisColor;
            double axisWidth;
            double axisFontSize;
            double axisLabelFontSize;
            JKQTPlabelPosition labelPos;
            JKQTPCAlabelType labelType;
            int digits;
            JKQTPCAdrawMode labelMode1;
            JKQTPCAdrawMode labelMode2;

            int minTicks;
            int minorTicks;
            bool axis0;

            double TickSpacing;
            double TickSpacingLog;
            bool minorTickLabelsOnlyDigit;
            bool LinTicksForLogAxis;
            bool minorTickLabels;
            bool AutoTicks;
            double AxisTickWidth;
            double AxisMinorTickWidth;
            bool AxisInverted;
            double TickInsideLength;
            double TickOutsideLength;
            double MinorTickInsideLength;
            double MinorTickOutsideLength;
            double labelAngel;
            double zeroAxisLineWidth;
            double axis1LineWidth;

            int columnNamedTickNames;
            int columnNamedTickValues;

        };



        struct PlotInfo {
            PlotInfo();
            QString title;
            bool grid;
            bool gridMinor;
            bool showKey;
            bool showTitle;

            bool keepAxisAspectRatio;
            bool keepDataAspectRatio;
            double dataAspectRatio;
            double axisAspectRatio;

            QColor axisColor;
            double axisWidth;
            QString fontName;
            double keyFontSize;
            double keyBoxLineWidth;
            QColor keyBackgroundColor;
            bool keyBox;            
            QColor keyLineColor;

            double keyXMargin;
            double keyYMargin;
            double keyXOffset;
            double keyYOffset;
            double keyXSeparation;
            double keyYSeparation;
            double key_line_length;

            double labelFontSize;
            double axisFontSize;
            double axisMinorFontSize;
            double axisLabelFontSize;
            double keyTransparency;
            JKQTPkeyPosition keyPosition;
            JKQTPkeyLayout keyLayout;
            QList<GraphInfo> graphs;



            QColor backgroundColor;
            QColor gridColor;
            Qt::PenStyle gridStyle;
            double gridWidth;
            QColor gridColorMinor;
            Qt::PenStyle gridStyleMinor;
            double gridWidthMinor;

            bool gridMinorX;
            bool gridMinorY;
            bool gridMajorX;
            bool gridMajorY;

            AxisInfo xAxis;
            AxisInfo yAxis;

            bool graphAutosize;
            int graphWidth;
            int graphHeight;

        };


        /** \brief class constructor, initialises as specified, the ID is obtained from the parent project */
        QFRDRTable(QFProject* parent);

        /** Default destructor */
        virtual ~QFRDRTable();

        /** \brief returns the used datamodel */
        QFTablePluginModel* model() ;
        /** \brief returns the table contents at the given position */
        QVariant getModelData(quint32 row, quint32 column);

        /** \brief indicates, whether the next of same role/previous of same role buttons should be displayed */
        virtual bool showNextPreviousOfSameRoleButton() const;


        virtual QVariant tableGetData(quint32 row, quint32 column) const;
        virtual void tableSetData(quint32 row, quint32 column, const QVariant& data);
        virtual void tableSetColumnTitle( quint32 column, const QString& data);
        virtual QString tableGetColumnTitle(quint32 column) const;
        virtual bool tableSupportsExpressions() const;
        virtual void tableSetExpression(quint32 row, quint32 column, const QString& expression=QString(""));
        virtual void tableSetColumnExpression(quint32 column, const QString& expression=QString(""));
        virtual void tableSetColumnComment(quint32 column, const QString& comment=QString(""));
        virtual void tableSetComment(quint32 row, quint32 column, const QString& comment=QString(""));
        virtual QString tableGetExpression(quint32 row, quint32 column) const;
        virtual void tableReevaluateExpressions();
        virtual int tableGetColumnCount() const;
        virtual int tableGetRowCount() const;
        virtual bool tableIsReadonly() const;
        virtual void tableSetColumnData(quint32 column, const QList<QVariant>& data);
        virtual QList<QVariant> tableGetColumnData(quint32 column) const;
        virtual void tableSetColumnDataAsDouble(quint32 column, const QVector<double>& data);
        virtual QVector<double> tableGetColumnDataAsDouble(int column) const;
        virtual bool tableGetDoEmitSignals() const;
        virtual void tableSetDoEmitSignals(bool doEmit);
        virtual void tableSetPreEvaluationExpression(const QString& expression);
        virtual QString tableGetPreEvaluationExpression() const;




        virtual bool colgraphGetDoEmitSignals() const;
        virtual void colgraphSetDoEmitSignals(bool doEmit);
        virtual int colgraphAddBoxPlot(int plotid, Orientation orientation, int columnX, int columnMin, int columnQ25, int columnMedian, int columnMean, int columnQ75, int columnMax, const QString&  title);
        virtual int colgraphAddGraph(int plotid, int columnX, int columnY, ColumnGraphTypes type, const QString&  title);
        virtual int colgraphAddFunctionGraph(int plotid, const QString& expression, ColumnGraphTypes type, const QString&  title, int columnParam=-1);
        virtual int colgraphAddFunctionGraph(int plotid, const QString& expression, ColumnGraphTypes type, const QString&  title, const QVector<double>& params);
        virtual void colgraphSetFunctionGraph(int plotid, int graphid, const QString& expression, ColumnGraphTypes type, const QString&  title, int columnParam=-1);
        virtual void colgraphSetFunctionGraph(int plotid, int graphid, const QString& expression, ColumnGraphTypes type, const QString&  title, const QVector<double>& params);
        virtual int colgraphAddErrorGraph(int plotid, int columnX, int columnXError, int columnY, int columnYError, ColumnGraphTypes type, const QString&  title, ErrorGraphTypes errorStyle=egtBars);
        virtual void colgraphSetErrorGraphProperties(int plot, int graphid, int columnXError, int columnYError, ErrorGraphTypes errorStyle=egtBars);
        virtual void colgraphSetErrorGraphErrorColumnX(int plot, int graphid, int columnXError);
        virtual void colgraphSetErrorGraphErrorColumnY(int plot, int graphid, int columnYError);
        virtual void colgraphSetErrorGraphStyle(int plot, int graphid, ErrorGraphTypes errorStyle);
        virtual void colgraphSetErrorGraphErrorColumnXAsymmetric(int plot, int graphid, bool xErrorAsyymetric, int columnXErrorAsymmetric=-1);
        virtual void colgraphSetErrorGraphErrorColumnYAsymmetric(int plot, int graphid, bool yErrorAsyymetric, int columnYErrorAsymmetric=-1);

        virtual int colgraphAddImageGraph(int plotid, int imageColumn, ImageColorPalette palette, double x, double y, double width, double height, int Nx, const QString& title);
        virtual int colgraphAddImageMaskGraph(int plotid, int imageColumn, double x, double y, double width, double height, int Nx, const QString& title, QColor trueColor=QColor("black"), QColor falseColor=QColor("transparent"));
        virtual int colgraphAddRGBImageGrph(int plotid, int imageRColumn, int imageGColumn, int imageBColumn, double x, double y, double width, double height, int Nx, const QString& title);
        virtual int colgraphAddRangeGraph(int plotid, Orientation orientation, double rangeStart, double rangeEnd, double rangeCenter, const QString& title, bool invertRange=false, bool fillrange=true, bool drawRangeLines=true, bool drawRangeCenter=true, QColor centerColor=QColor("red"),  Qt::PenStyle centerStyle=Qt::SolidLine, double centerWidth=1.0);
        virtual void colgraphSetImageGraphModifier(int plotid, int graphid, int imageModifierColumn, ImageModifierMode mode);
        virtual void colgraphSetImageGraphRange(int plotid, int graphid, ImageGraphRangeChannel channel, bool autoRange, double min=0, double max=0);
        virtual void colgraphSetGraphProperty(int plotid, int graphid, const QString& name, const QVariant& value);
        virtual QVariant colgraphGetGraphProperty(int plotid, int raphid, const QString& name, const QVariant& defaultValue=QVariant());

        virtual int colgraphAddPlot(const QString&  title, const QString& xLabel=QString("x"), const QString& yLabel=QString("y"), bool logX=false, bool logY=false);
        virtual int colgraphGetGraphCount(int plotid) const;
        virtual int colgraphGetPlotCount() const;
        virtual void colgraphRemovePlot(int plotid) ;
        virtual void colgraphRemoveGraph(int plotid, int graphid);

        virtual void colgraphSetGraphWhiskerLineStyle(int plot,  int graph,  Qt::PenStyle  style);
        virtual void colgraphSetGraphLineStyle(int plotid,  int graphid,  Qt::PenStyle  style);
        virtual void colgraphSetGraphLineWidth(int plotid,  int graphid,  double width);
        virtual void colgraphSetGraphFillStyle(int plotid,  int graphid,  Qt::BrushStyle  style);
        virtual void colgraphSetGraphWidth(int plot,  int graph,  double width);
        virtual void colgraphSetGraphShift(int plot,  int graph,  double shift);


        virtual void colgraphSetGraphColor(int plotid,  int graphid, QColor color);
        virtual void colgraphSetGraphColor(int plotid,  int graphid, QColor  color, QColor  fillColor);
        virtual void colgraphSetGraphColor(int plotid,  int graphid, QColor  color, QColor  fillColor, QColor errorColor);
        virtual void colgraphSetPlotTitle(int plotid, const QString&  title);
        virtual QString colgraphGetPlotTitle(int plotid);
        virtual void colgraphSetPlotXAxisProps(int plotid, const QString& xLabel=QString("x"), bool logX=false);
        virtual void colgraphSetPlotYAxisProps(int plotid, const QString& yLabel=QString("y"), bool logY=false);
        virtual void colgraphSetPlotXRange(int plotid, double xmin, double xmax);
        virtual void colgraphSetPlotYRange(int plotid, double xmin, double xmax);

        virtual void colgraphSetGraphTitle(int plotid,  int graphid,  const QString& title);
        virtual void colgraphSetGraphType(int plotid,  int graphid,  ColumnGraphTypes type);
        virtual void colgraphSetGraphErrorStyle(int plotid,  int graphid,  ErrorGraphTypes errorStyle);
        virtual void colgraphSetGraphErrorColor(int plotid,  int graphid, QColor errorColor);
        virtual void colgraphSetGraphFillColor(int plotid,  int graphid, QColor fillColor);
        virtual void colgraphSetGraphErrorTransparency(int plotid,  int graphid, double errorT);
        virtual void colgraphSetGraphFillTransparency(int plotid,  int graphid, double fillT);
        virtual void colgraphSetGraphTransparency(int plotid,  int graphid, double trans);
        virtual void colgraphSetGraphSymbol(int graph,  int plot,  ColumnGraphSymbols symbol, double symbolSize=10.0);
        virtual int colgraphAddParametrizedScatterGraph(int plot, int columnX, int columnY, int columnP, const QString&  title, ParametrizationType type=cgptColorLines, bool colorbarVisible=false, const QString& colorbarLabel=QString());
        virtual QString colgraphGetPlotXAxisLabel(int plot);
        virtual QString colgraphGetPlotYAxisLabel(int plot);
        virtual bool colgraphGetPlotXAxisLog(int plot);
        virtual bool colgraphGetPlotYAxisLog(int plot);
        virtual void colgraphSetColorPalette(int plotid, int graphid, ImageColorPalette palette);
        void colgraphToolsSetGraphtype(QFRDRTable::GraphInfo& g, QFRDRColumnGraphsInterface::ColumnGraphTypes type);


        virtual int userCurvesGetCount() const;
        virtual QString userCurvesGetName(int index) const;
        virtual QVector<double> userCurvesGetX(int index) const;
        virtual QVector<double> userCurvesGetXError(int index) const;
        virtual QVector<double> userCurvesGetY(int index) const;
        virtual QVector<double> userCurvesGetYError(int index) const;
        virtual bool userCurvesGetLogX(int index) const;
        virtual bool userCurvesGetLogY(int index) const;
        virtual CurveType userCurvesGetType(int index) const;
        virtual QString userCurvesGetXLabel(int index) const;
        virtual QString userCurvesGetYLabel(int index) const;


        int getPlotCount() const;
        PlotInfo getPlot(int i) const;
        void addPlot();
        void addPlot(PlotInfo plotInfo);
        void setPlot(int i, PlotInfo plotInfo);
        void deletePlot(int i);
        void swapPlots(int i, int j);


        /** \brief return type (short type string) */
        virtual QString getType() const { return "table"; };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("Data Table"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/table/projecttree_tablefile.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("data table"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/table/projecttree_tablefile.png"); };
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount();
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i);
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i=0, QWidget* parent=NULL);
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() const {
            QStringList sl;
            sl << "QFTABLEXML";
            QStringList f=QFDataExportHandler::getFormats();
            for (int i=0; i<f.size(); i++) sl<<QString("F%1").arg(i);
            return sl;

        }
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() const { return tr("Export Data Table ..."); }
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() const { return tr("Table XML file (*.qftxml)")+";;"+QFDataExportHandler::getFormats().join(";;"); }

        QVariant evaluateExpression(QFMathParser &mp, QFMathParser::qfmpNode *n, QModelIndex cell, bool *ok, const QString &expression, QString *error, bool columnMode=false);
        void readAxisInfo(AxisInfo &plot, const QString &axisName, QDomElement te, bool readLabels=true);
        void readPlotInfo(PlotInfo &plot, QDomElement te, bool readLabels=true);
        void readGraphInfo(GraphInfo &graph, QDomElement te);
        void writeAxisInfo(QXmlStreamWriter& w, const AxisInfo &plot, const QString &axisName) const;
        void writePlotInfo(QXmlStreamWriter& w, const PlotInfo &plot, bool writeGraphs=true) const;
        void writeGraphInfo(QXmlStreamWriter& w, const GraphInfo &graph) const;
        void loadColumnToComboBox(QComboBox *combo);

        QString getParserPreScript() const;
    public slots:
        void setParserPreScript(const QString& script);
        void emitRebuildPlotWidgets();

        void columnsInserted(int start, int count, bool emitRebuild=true);
        void columnsRemoved(int start, int count, bool emitRebuild=true);

    protected slots:
        void tdataChanged( const QModelIndex & /*tl*/, const QModelIndex & /*br*/ ) {
            emit rawDataChanged();
        }
        void trawDataChanged() {
            emit rawDataChanged();
        }


        void didRebuildPlotWidgets();
    protected:
        bool emittedRebuildPlotWidgets;
    signals:
        void rebuildPlotWidgets();


    protected:
        virtual void intWriteData(QXmlStreamWriter& w) const;
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);

        /** \brief stores a table of QVariants */
        QFTablePluginModel* datamodel;
        QList<PlotInfo> plots;
        QList<QColor> autocolors;

        bool emitColGraphChangedSignals;

    private:
};


#endif // QFRDRTABLE_H
