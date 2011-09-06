#include "qfrawdatarecord.h"
#include "qftools.h"
#include <QtXml>
#include "qtriple.h"

QFRawDataRecord::QFRawDataRecord(QFProject* parent):
    QObject(parent), QFProperties()
{
    project=parent;
    errorOcc=false;
    errorDesc="";
    name="";
    description="";
    resultsmodel=NULL;
    propModel=NULL;
    doEmitResultsChanged=true;
}


QFRDRPropertyModel* QFRawDataRecord::getPropertyModel() {
    if (propModel==NULL) {
        propModel=new QFRDRPropertyModel();
        propModel->init(this);
    }
    return propModel;
}

QFRDRResultsModel* QFRawDataRecord::resultsGetModel() {
    if (resultsmodel==NULL) {
        resultsmodel=new QFRDRResultsModel();
        resultsmodel->init(this);
    }
    return resultsmodel;
};

void QFRawDataRecord::init(QString name, QStringList inputFiles) {
    this->ID=project->getNewID();
    this->name=name;
    description="";
    files=inputFiles;
    intReadData();
    //std::cout<<"after intReadData() in init ...\n";
    project->registerRawDataRecord(this);
    //std::cout<<"init ended ...\n";
}


void QFRawDataRecord::init(QDomElement& e) {
    //std::cout<<"creating QFRawDataRecord\n";
    name="";;
    description="";
    //std::cout<<"  reading XML\n";
    readXML(e);
    //std::cout<<"  registering record\n";
    project->registerRawDataRecord(this);
    //std::cout<<"created QFRawDataRecord\n";
}

QFRawDataRecord::~QFRawDataRecord() {
    //std::cout<<"deleting QFRawDataRecord\n";
    //std::cout<<"deleting QFRawDataRecord ... OK\n";
}

void QFRawDataRecord::readXML(QDomElement& e) {
    bool ok=true;
    name=e.attribute("name", "rawdatarecord");
    ID=e.attribute("id", "-1").toInt(&ok);
    if (ID==-1) { setError(tr("invalid ID in <rawdatarecord name=\"%1\" ...>!").arg(name)); return; }
    if (!project->checkID(ID)) {
        setError(tr("ID %1 in <rawdatarecord name=\"%2\" ...> already in use in the project!").arg(ID).arg(name));
        return;
    }
    QDomElement te=e.firstChildElement("description");
    if (te.isNull()) { description=""; } else { description=te.text(); }
    //std::cout<<"    reading XML: properties\n";
    props.clear();
    te=e.firstChildElement("properties");
    readProperties(te);

    results.clear();
    te=e.firstChildElement("results");
    if (!te.isNull()) {
        te=te.firstChildElement("evaluation");
        while (!te.isNull()) {
            QString en=te.attribute("name");
            QString group=te.attribute("group");
            int groupIndex=te.attribute("groupindex", "0").toInt();
            QString description=te.attribute("description");
            QDomElement re=te.firstChildElement("result");
            results[en].group=group;
            results[en].groupIndex=groupIndex;
            results[en].description=description;
            while (!re.isNull()) {
                QString n=re.attribute("name", "");
                QString t=re.attribute("type", "invalid");
                evaluationResult r;
                r.group=re.attribute("group", "");
                r.label=re.attribute("label", "");
                r.label_rich=re.attribute("labelrich", "");
                //r.name=n;
                r.type=qfrdreInvalid;
                if (t=="boolean") {
                    r.type=qfrdreBoolean;
                    r.bvalue=QVariant(re.attribute("value", "false")).toBool();
                } else if (t=="integer") {
                    r.type=qfrdreInteger;
                    r.ivalue=re.attribute("value", "0").toInt();
                    r.unit=re.attribute("unit", "");
                } else if (t=="string") {
                    r.type=qfrdreString;
                    r.svalue=re.attribute("value", "");
                } else if (t=="number") {
                    r.type=qfrdreNumber;
                    r.dvalue=re.attribute("value", "0.0").toDouble();
                    r.unit=re.attribute("unit", "");
                } else if (t=="numberlist") {
                    r.type=qfrdreNumberVector;
                    r.dvec.clear();
                    QStringList s=re.attribute("value", "0.0").split(";");
                    for (int i=0; i<s.size(); i++) {
                        bool ok=false;
                        double d=s[i].toDouble(&ok);
                        if (ok) { r.dvec.append(d); }
                        else { r.dvec.append(0); }
                    }
                    r.unit=re.attribute("unit", "");
                } else if (t=="numbermatrix") {
                    r.type=qfrdreNumberVector;
                    r.dvec.clear();
                    QStringList s=re.attribute("value", "0.0").split(";");
                    for (int i=0; i<s.size(); i++) {
                        bool ok=false;
                        double d=s[i].toDouble(&ok);
                        if (ok) { r.dvec.append(d); }
                        else { r.dvec.append(0); }
                    }
                    r.unit=re.attribute("unit", "");
                    r.columns=re.attribute("columns", "").toInt();
                } else if (t=="numbererror") {
                    r.type=qfrdreNumberError;
                    r.dvalue=re.attribute("value", "0.0").toDouble();
                    r.derror=re.attribute("error", "0.0").toDouble();
                    r.unit=re.attribute("unit", "");
                }
                if (!n.isEmpty() && !en.isEmpty()) results[en].results.insert(n, r);
                re=re.nextSiblingElement("result");
            }

            te = te.nextSiblingElement("evaluation");
        }
    }

    //std::cout<<"    reading XML: files\n";
    te=e.firstChildElement("files");
    files.clear();
    if (!te.isNull()) {
        QDomElement fe=te.firstChildElement("file");
        while (!fe.isNull()) {
            QString filexml=fe.text();
            QFileInfo fi(project->getFile());
            //std::cout<<"file = "<<filexml.toStdString()<<"\n";
            //std::cout<<"  project-absolute path = "<<fi.absoluteDir().absolutePath().toStdString()<<"\n";
            //std::cout<<"  file-absolute path = "<<fi.absoluteDir().absoluteFilePath(filexml).toStdString()<<"\n";

            files.push_back(fi.absoluteDir().absoluteFilePath(filexml));
            fe=fe.nextSiblingElement("file");
        }
    }
    //std::cout<<"    reading XML: data\n";
    te=e.firstChildElement("data");
    intReadData(&te);
    //std::cout<<"reading XML: done!\n";
    if (!errorOcc) {
        emit propertiesChanged();
        emit rawDataChanged();
    }
    if (resultsmodel) resultsmodel->init(this);
}


void QFRawDataRecord::writeXML(QXmlStreamWriter& w) {
    w.writeStartElement("rawdataelement");
    w.writeAttribute("type", getType());
    w.writeAttribute("name", name);
    w.writeAttribute("id", QString::number(ID));
    w.writeStartElement("description");
    w.writeCDATA(description);
    w.writeEndElement();
    w.writeStartElement("properties");
    storeProperties(w);
    w.writeEndElement();
    w.writeStartElement("results");
    QMapIterator<QString, evaluationIDMetadata > i(results);
    while (i.hasNext()) {
    //for (int i=0; i<results.keys().size(); i++) {
        i.next();
        w.writeStartElement("evaluation");
        QString n=i.key();
        w.writeAttribute("name", n);
        w.writeAttribute("group", i.value().group);
        w.writeAttribute("groupindex", QString::number(i.value().groupIndex));
        w.writeAttribute("description", i.value().description);
        QMapIterator<QString, evaluationResult> j(i.value().results);
        //for (int j=0; j<i.value().size(); j++) {
        while (j.hasNext()) {
            j.next();
            w.writeStartElement("result");
            QString rn=j.key();
            w.writeAttribute("name", rn);            
            evaluationResult r=j.value();
            if (!r.label.isEmpty()) w.writeAttribute("label", r.label);
            if (!r.group.isEmpty()) w.writeAttribute("group", r.group);
            if (!r.label_rich.isEmpty()) w.writeAttribute("labelrich", r.label_rich);
            switch(r.type) {
                case qfrdreInvalid:
                    w.writeAttribute("type", "invalid");
                    break;
                case qfrdreBoolean:
                    w.writeAttribute("type", "boolean");
                    w.writeAttribute("value", (r.bvalue)?QString("true"):QString("false"));
                    break;
                case qfrdreInteger:
                    w.writeAttribute("type", "integer");
                  #ifdef Q_OS_WIN32
                    w.writeAttribute("value", QLocale::c().toString(r.ivalue));
                  #else
                    w.writeAttribute("value", QLocale::c().toString(r.ivalue));
                  #endif
                    w.writeAttribute("unit", r.unit);
                    break;
                case qfrdreString:
                    w.writeAttribute("type", "string");
                    w.writeAttribute("value", r.svalue);
                    break;
                case qfrdreNumber:
                    w.writeAttribute("type", "number");
                    w.writeAttribute("value", QLocale::c().toString(r.dvalue));
                    w.writeAttribute("unit", r.unit);
                    break;
                case qfrdreNumberVector: {
                    w.writeAttribute("type", "numberlist");
                    QString s="";
                    for (int i=0; i<r.dvec.size(); i++) {
                        if (i>0) s=s+";";
                        s=s+QLocale::c().toString(r.dvec[i]);
                    }
                    w.writeAttribute("value", s);
                    w.writeAttribute("unit", r.unit);
                    } break;
                case qfrdreNumberMatrix: {
                    w.writeAttribute("type", "numbermatrix");
                    QString s="";
                    for (int i=0; i<r.dvec.size(); i++) {
                        if (i>0) s=s+";";
                        s=s+QLocale::c().toString(r.dvec[i]);
                    }
                    w.writeAttribute("value", s);
                    w.writeAttribute("unit", r.unit);
                    w.writeAttribute("columns", QLocale::c().toString(r.columns));
                    } break;
                case qfrdreNumberError:
                    w.writeAttribute("type", "numbererror");
                    w.writeAttribute("value", QLocale::c().toString(r.dvalue));
                    w.writeAttribute("error", QLocale::c().toString(r.derror));
                    w.writeAttribute("unit", r.unit);
                    break;

            }
            w.writeEndElement();
        }
        w.writeEndElement();
    }
    w.writeEndElement();
    if (files.size()>0) {
        w.writeStartElement("files");
        for (int i=0; i< files.size(); i++) {
            QString file=files[i];
            QFileInfo fi(project->getFile());
            file=fi.absoluteDir().relativeFilePath(files[i]);
            w.writeTextElement("file", file);
        }
        w.writeEndElement();
    }
    w.writeStartElement("data");
    intWriteData(w);
    w.writeEndElement();
    w.writeEndElement();
}





void QFRawDataRecord::resultsSetNumber(QString evaluationName, QString resultName, double value, QString unit) {
    evaluationResult r;
    r.type=qfrdreNumber;
    r.dvalue=value;
    r.unit=unit;
    results[evaluationName].results.insert(resultName, r);
    if (doEmitResultsChanged) emit resultsChanged();
};

void QFRawDataRecord::resultsRemove(QString evalName, QString resultName, bool emitChangedSignal) {
    if (results.contains(evalName)) {
        bool changed=false;
        if (results[evalName].results.remove(resultName)>0) changed=true;
        if (results[evalName].results.isEmpty()) {
            results.remove(evalName);
            changed=true;
        }
        if (changed && emitChangedSignal && doEmitResultsChanged) emit resultsChanged();
    }
}

void QFRawDataRecord::resultsClear(QString name, QString postfix) {
    if (results.contains(name)) {
        QMapIterator<QString, evaluationResult> i(results[name].results);
        while (i.hasNext()) {
            i.next();
            //cout << i.key() << ": " << i.value() << endl;
            if (i.key().endsWith(postfix)) results[name].results.remove(i.key());
        }
        if (doEmitResultsChanged) emit resultsChanged();
    }
};

void QFRawDataRecord::resultsSetNumberList(QString evaluationName, QString resultName, QVector<double>& value, QString unit) {
    evaluationResult r;
    r.type=qfrdreNumberVector;
    r.dvec=value;
    r.unit=unit;
    results[evaluationName].results.insert(resultName, r);
    if (doEmitResultsChanged) emit resultsChanged();
};

void QFRawDataRecord::resultsSetNumberMatrix(QString evaluationName, QString resultName, QVector<double>& value, int columns, QString unit) {
    evaluationResult r;
    r.type=qfrdreNumberMatrix;
    r.dvec=value;
    r.unit=unit;
    r.columns=columns;
    results[evaluationName].results.insert(resultName, r);
    if (doEmitResultsChanged) emit resultsChanged();
};

void QFRawDataRecord::resultsSetNumberList(QString evaluationName, QString resultName, double* value, int items, QString unit) {
    QVector<double> data;
    for (int i=0; i<items; i++) {
        data.append(value[i]);
    }
    resultsSetNumberList(evaluationName, resultName, data, unit);
}

void QFRawDataRecord::resultsSetNumberMatrix(QString evaluationName, QString resultName, double* value, int columns, int rows, QString unit) {
    QVector<double> data;
    for (int i=0; i<columns*rows; i++) {
        data.append(value[i]);
    }
    resultsSetNumberMatrix(evaluationName, resultName, data, columns, unit);
}



void QFRawDataRecord::resultsSetNumberError(QString evaluationName, QString resultName, double value, double error, QString unit)  {
    evaluationResult r;
    r.type=qfrdreNumberError;
    r.dvalue=value;
    r.derror=error;
    r.unit=unit;
    results[evaluationName].results.insert(resultName, r);
    if (doEmitResultsChanged) emit resultsChanged();
};

void QFRawDataRecord::resultsSetInteger(QString evaluationName, QString resultName, int64_t value, QString unit) {
    evaluationResult r;
    r.type=qfrdreInteger;
    r.ivalue=value;
    r.unit=unit;
    results[evaluationName].results.insert(resultName, r);
    if (doEmitResultsChanged) emit resultsChanged();
};

void QFRawDataRecord::resultsSetString(QString evaluationName, QString resultName, QString value) {
    evaluationResult r;
    r.type=qfrdreString;
    r.svalue=value;
    results[evaluationName].results.insert(resultName, r);
    if (doEmitResultsChanged) emit resultsChanged();
};

void QFRawDataRecord::resultsSetLabel(QString evaluationName, QString resultName, QString label, QString label_rich) {
    if (!resultsExists(evaluationName, resultName)) return;
    evaluationResult r=resultsGet(evaluationName, resultName);
    r.label=label;
    r.label_rich=label_rich;
    results[evaluationName].results[resultName]=r;
    if (doEmitResultsChanged) emit resultsChanged();
}

void QFRawDataRecord::resultsSetGroup(QString evaluationName, QString resultName, QString group) {
    if (!resultsExists(evaluationName, resultName)) return;
    evaluationResult r=resultsGet(evaluationName, resultName);
    r.group=group;
    results[evaluationName].results[resultName]=r;
    if (doEmitResultsChanged) emit resultsChanged();
}

void QFRawDataRecord::resultsSetEvaluationGroupIndex(QString evaluationName, int64_t groupIndex) {
    results[evaluationName].groupIndex=groupIndex;
}

int64_t QFRawDataRecord::resultsGetEvaluationGroupIndex(QString evaluationName) const {
    if (results.contains(evaluationName)) {
        return results[evaluationName].groupIndex;
    }
    return -1;
}

void QFRawDataRecord::resultsSetEvaluationGroup(QString evaluationName, QString group) {
    results[evaluationName].group=group;
}

QString QFRawDataRecord::resultsGetEvaluationGroup(QString evaluationName) const {
    if (results.contains(evaluationName)) {
        QString g=results[evaluationName].group;
        return g;
    }
    return "";
}

void QFRawDataRecord::resultsSetEvaluationDescription(QString evaluationName, QString description) {
    results[evaluationName].description=description;
}

QString QFRawDataRecord::resultsGetEvaluationDescription(QString evaluationName) const {
    if (results.contains(evaluationName)) {
        QString g=results[evaluationName].description;
        if (g.isEmpty()) return evaluationName;
        else return g;
    }
    return "";
}

void QFRawDataRecord::resultsSetBoolean(QString evaluationName, QString resultName, bool value) {
    evaluationResult r;
    r.type=qfrdreBoolean;
    r.bvalue=value;
    results[evaluationName].results.insert(resultName, r);
    if (doEmitResultsChanged) emit resultsChanged();
};

void QFRawDataRecord::resultsSet(QString evaluationName, QString resultName, const evaluationResult& result) {
    results[evaluationName].results.insert(resultName, result);
    if (doEmitResultsChanged) emit resultsChanged();
}

QVariant QFRawDataRecord::resultsGetAsQVariant(QString evalName, QString resultName) const {
    QVariant result;
    evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreBoolean: result=r.bvalue; break;
        case qfrdreInteger: result=(qlonglong)r.ivalue; break;
        case qfrdreNumberError: result=QPointF(r.dvalue, r.derror); break;
        case qfrdreNumber: result=r.dvalue; break;
        case qfrdreNumberMatrix:
        case qfrdreNumberVector: {
            QList<QVariant> data;
            for (int i=0; i<r.dvec.size(); i++) {
                data.append(r.dvec[i]);
            }
            result=data;
            break;
        }
        case qfrdreString: result=r.svalue; break;
        default: result=QVariant(); break;
    }
    return result;
}

QString QFRawDataRecord::resultsGetAsString(QString evalName, QString resultName) const {
    evaluationResult r=resultsGet(evalName, resultName);
    switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return tr("true"); else return tr("false");
        case qfrdreInteger: return tr("%1 %2").arg(r.ivalue).arg(r.unit);
        case qfrdreNumber: return tr("%1 %2").arg(r.dvalue).arg(r.unit);
        case qfrdreNumberVector: {
            QString s="(";
            for (int i=0; i<r.dvec.size(); i++) {
                if (i>0) s=s+"; ";
                s=s+QString::number(r.dvec[i]);
            }
            return s+") "+r.unit;
        }
        case qfrdreNumberMatrix: {
            QString s="(";
            for (int i=0; i<r.dvec.size(); i++) {
                if (i>0) {
                    if (i%r.columns==0) s=s+";; ";
                    else s=s+"; ";
                }
                s=s+QString::number(r.dvec[i]);
            }
            return s+") "+r.unit;
        }
        case qfrdreNumberError: return tr("(%1 +/- %2) %3").arg(r.dvalue).arg(r.derror).arg(r.unit);
        case qfrdreString: return r.svalue;
        default: return ("");
    }
    return QString("");
}

double QFRawDataRecord::resultsGetAsDouble(QString evalName, QString resultName, bool* ok) const {
    evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return 1; else return 0;
        case qfrdreInteger: return r.ivalue;
        case qfrdreNumber: case qfrdreNumberError: return r.dvalue;
        case qfrdreString: return r.svalue.toDouble(ok);
        default: if (ok) *ok=false;
                 return 0.0;

    }
    if (ok) *ok=false;
    return 0.0;
}

int64_t QFRawDataRecord::resultsGetAsInteger(QString evalName, QString resultName, bool* ok) const {
    evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
     switch(r.type) {
        case qfrdreBoolean: if (r.bvalue) return 1; else return 0;
        case qfrdreInteger: return r.ivalue;
        case qfrdreNumber: case qfrdreNumberError: return r.dvalue;
        case qfrdreString: return r.svalue.toInt(ok);
        default: if (ok) *ok=false;
                 return 0.0;
    }
    if (ok) *ok=false;
    return 0.0;
}

bool QFRawDataRecord::resultsGetAsBoolean(QString evalName, QString resultName, bool* ok) const {
    evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
     switch(r.type) {
        case qfrdreBoolean: return r.bvalue;
        case qfrdreInteger: return r.ivalue!=0;
        case qfrdreNumber: case qfrdreNumberError: return r.dvalue!=0.0;
        case qfrdreString: return QStringToBool(r.svalue);
        default: if (ok) *ok=false;
                 return false;
    }
    if (ok) *ok=false;
    return false;
}

QVector<double> QFRawDataRecord::resultsGetAsDoubleList(QString evalName, QString resultName, bool* ok) const {
    evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreNumberVector:
        case qfrdreNumberMatrix: return r.dvec;
        default: if (ok) *ok=false;
                 return QVector<double>();
    }
    if (ok) *ok=false;
    return QVector<double>();
}

double QFRawDataRecord::resultsGetErrorAsDouble(QString evalName, QString resultName, bool* ok) const {
    evaluationResult r=resultsGet(evalName, resultName);
    if (ok) *ok=true;
    switch(r.type) {
        case qfrdreNumber: case qfrdreNumberError: return r.derror;
        default: if (ok) *ok=false;
                 return 0.0;
    }
    if (ok) *ok=false;
    return 0.0;
};



QString QFRawDataRecord::resultsGetLabel(QString evaluationName, QString resultName) const {
    evaluationResult r=resultsGet(evaluationName, resultName);
    if (r.label.isEmpty()) return resultName;
    else return r.label;
}


QString QFRawDataRecord::resultsGetGroup(QString evaluationName, QString resultName) const {
    evaluationResult r=resultsGet(evaluationName, resultName);
    return r.group;
}

QString QFRawDataRecord::resultsGetLabelRichtext(QString evaluationName, QString resultName) const {
    evaluationResult r=resultsGet(evaluationName, resultName);
    if (r.label_rich.isEmpty()) return r.label;
    else if (r.label_rich.isEmpty()) return resultName;
    else return r.label_rich;
}


QString QFRawDataRecord::resultsGetResultName(QString evaluationName, int i) const {
    if (results.contains(evaluationName)) {
        QList<QString> r=results[evaluationName].results.keys();
        if (i<r.size()) {
            return r.at(i);
        }
    }
    return QString("");
}

void QFRawDataRecord::resultsCopy(QString oldEvalName, QString newEvalName) {
    if (resultsExistsFromEvaluation(oldEvalName)) {
        QMapIterator<QString, evaluationResult> i(results[oldEvalName].results);
        while (i.hasNext()) {
            i.next();
            results[newEvalName].results.insert(i.key(), i.value());
        }
        if (doEmitResultsChanged) emit resultsChanged();
    }
}

bool QFRawDataRecord::resultsSaveToCSV(QString filename, QString separator, QChar decimalPoint, QChar stringDelimiter) {
    QString sdel=stringDelimiter;
    QString dp=decimalPoint;
    QStringList rownames=resultsCalcNames();
    QStringList header, data;
    header.append(sdel+tr("datafield")+sdel);
    header.append("");
    QLocale loc=QLocale::c();
    for (int i=0; i<rownames.size(); i++) data.append(sdel+rownames[i]+sdel);
    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        header[0] += separator+sdel+evalname+sdel;
        header[1] += separator+sdel+tr("value")+sdel;
        bool hasError=false;
        for (int r=0; r<rownames.size(); r++) {
            QString dat="";
            if (resultsExists(evalname, rownames[r])) {
                switch(resultsGet(evalname, rownames[r]).type) {
                    case qfrdreNumber: dat=doubleToQString(resultsGetAsDouble(evalname, rownames[r]), 15, 'g', decimalPoint); break;
                    case qfrdreNumberError: dat=doubleToQString(resultsGetAsDouble(evalname, rownames[r]), 15, 'g', decimalPoint); hasError=true; break;
                    case qfrdreInteger: dat=loc.toString((qlonglong)resultsGetAsInteger(evalname, rownames[r])); break;
                    case qfrdreBoolean: dat=(resultsGetAsBoolean(evalname, rownames[r]))?QString("1"):QString("0"); break;
                    case qfrdreString: dat=stringDelimiter+resultsGetAsString(evalname, rownames[r]).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r")+stringDelimiter; break;
                    case qfrdreNumberVector:
                    case qfrdreNumberMatrix: dat=stringDelimiter+resultsGetAsString(evalname, rownames[r]).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r")+stringDelimiter; break;
                    default: break;
                }
            }
            data[r]+=separator+dat;
        }
        if (hasError) {
            header[0] += separator;
            header[1] += separator+sdel+tr("error")+sdel;
            for (int r=0; r<rownames.size(); r++) {
                QString dat="";
                if (resultsExists(evalname, rownames[r])) {
                    if (resultsGet(evalname, rownames[r]).type==qfrdreNumberError) {
                        dat=doubleToQString(resultsGetErrorAsDouble(evalname, rownames[r]), 15, 'g', decimalPoint);
                    }
                }
                data[r]+=separator+dat;
            }
        }
    }

     QFile of(filename);
     if (of.open(QFile::WriteOnly | QFile::Truncate)) {
         QTextStream out(&of);
         QTextCodec* c=QTextCodec::codecForName("ISO-8859-1");
         if (c==NULL) c=QTextCodec::codecForCStrings();
         if (c==NULL) c=QTextCodec::codecForLocale();
         out.setCodec(c);
         for (int i=0; i<header.size(); i++) out<<header[i]<<"\n";
         for (int i=0; i<data.size(); i++) out<<data[i]<<"\n";
     } else { return false; }
     return true;
}


bool QFRawDataRecord::resultsSaveToSYLK(QString filename) {
    // try output SYLK file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
    out.setLocale(QLocale::c());


    // write SYLK header
    out<<"ID;P\n";

    QChar stringDelimiter='"';
    QStringList rownames=resultsCalcNames();
    // write column headers
    out<<QString("C;Y1;X1;K\"%1\"\n").arg(tr("datafield"));
    for (int r=0; r<rownames.size(); r++) {
        out<<QString("C;Y%2;X1;K\"%1\"\n").arg(rownames[r]).arg(r+3);
        //out<<QString("F;Y%2;X1;SDB\n").arg(r+2);
    }

    QLocale loc=QLocale::c();
    int col=2;
    for (int c=0; c<resultsGetEvaluationCount(); c++) {
        QString evalname=resultsGetEvaluationName(c);
        out<<QString("C;Y1;X%2;K\"%1\"\n").arg(evalname).arg(col);
        //out<<QString("F;Y1;X%2;SDB\n").arg(col);
        out<<QString("C;Y2;X%2;K\"%1\"\n").arg(tr("value")).arg(col);
        //out<<QString("F;Y2;X%2;SDB\n").arg(col);
        bool hasError=false;
        for (int r=0; r<rownames.size(); r++) {
            QString dat="";
            if (resultsExists(evalname, rownames[r])) {
                switch(resultsGet(evalname, rownames[r]).type) {
                    case qfrdreNumber: dat=loc.toString(resultsGetAsDouble(evalname, rownames[r]), 'g', 15); break;
                    case qfrdreNumberError: dat=loc.toString(resultsGetAsDouble(evalname, rownames[r]), 'g', 15); hasError=true; break;
                    case qfrdreInteger: dat=loc.toString((qlonglong)resultsGetAsInteger(evalname, rownames[r])); break;
                    case qfrdreBoolean: dat=(resultsGetAsBoolean(evalname, rownames[r]))?QString("1"):QString("0"); break;
                    case qfrdreString: dat=stringDelimiter+resultsGetAsString(evalname, rownames[r]).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r")+stringDelimiter; break;
                    case qfrdreNumberVector:
                    case qfrdreNumberMatrix: dat=stringDelimiter+resultsGetAsString(evalname, rownames[r]).replace(stringDelimiter, "\\"+QString(stringDelimiter)).replace('\n', "\\n").replace('\r', "\\r")+stringDelimiter; break;
                    default: break;
                }
            }
            if (!dat.isEmpty()) out<<QString("C;X%1;Y%2;N;K%3\n").arg(col).arg(r+3).arg(dat);
        }
        if (hasError) {
            col++;
            out<<QString("C;Y2;X%2;K\"%1\"\n").arg(tr("error")).arg(col);
            //out<<QString("F;Y2;X%2;SDB\n").arg(col);
            for (int r=0; r<rownames.size(); r++) {
                QString dat="";
                if (resultsExists(evalname, rownames[r])) {
                    if (resultsGet(evalname, rownames[r]).type==qfrdreNumberError) {
                        dat=loc.toString(resultsGetErrorAsDouble(evalname, rownames[r]), 'g', 15);;
                    }
                }
                if (!dat.isEmpty()) out<<QString("C;X%1;Y%2;N;K%3\n").arg(col).arg(r+3).arg(dat);
            }
        }
        col++;
    }
    return true;
}


bool QFRawDataRecord_StringPairCaseInsensitiveCompare(const QPair<QString, QString> &s1, const QPair<QString, QString> &s2) {
    return s1.first.toLower() < s2.first.toLower();
}

bool QFRawDataRecord_StringTripleCaseInsensitiveCompare(const QTriple<QString, QString, QString> &s1, const QTriple<QString, QString, QString> &s2) {
    return s1.first.toLower() < s2.first.toLower();
}


/*QList<QString> QFRawDataRecord::resultsCalcNames() const {
    QStringList l;
    //if (record) {
        int evalCount=resultsGetEvaluationCount();
        for (int i=0; i<evalCount; i++) {
            QString en=resultsGetEvaluationName(i);
            int jmax=resultsGetCount(en);
            for (int j=0; j<jmax; j++) {
                QString rn=resultsGetResultName(en, j);
                if (!l.contains(rn)) {
                    l.append(rn);
                }
            }
        }
    //}
    if (l.size()>0) l.sort();
    return l;
}

QList<QString> QFRawDataRecord::resultsCalcGroups() const {
    QStringList l;
    //if (record) {
        int evalCount=resultsGetEvaluationCount();
        for (int i=0; i<evalCount; i++) {
            QString en=resultsGetEvaluationName(i);
            int jmax=resultsGetCount(en);
            for (int j=0; j<jmax; j++) {
                QString rn=resultsGetResultName(en, j);
                QString g=resultsGetGroup(en, rn);
                if (!l.contains(g)) {
                    l.append(g);
                }
            }
        }
    //}
    if (l.size()>0) l.sort();
    return l;
}


QList<QPair<QString, QString> > QFRawDataRecord::resultsCalcNamesAndLabels() const {
    QStringList l;
    QList<QPair<QString, QString> > list;
    //if (record) {
        int evalCount=resultsGetEvaluationCount();
        for (int i=0; i<evalCount; i++) {
            QString en=resultsGetEvaluationName(i);
            int jmax=resultsGetCount(en);
            for (int j=0; j<jmax; j++) {
                QString rn=resultsGetResultName(en, j);
                QString lab=resultsGetLabel(en, rn);
                if (!l.contains(lab)) {
                    l.append(rn);
                    list.append(qMakePair(lab, rn));
                }
            }
        }
    //}
    if (list.size()>0) {
        qSort(list.begin(), list.end(), QFRawDataRecord_StringPairCaseInsensitiveCompare);
    }

    return list;
}



QList<QPair<QString, QString> > QFRawDataRecord::resultsCalcNamesAndLabelsRichtext() const {
    QStringList l;
    QList<QTriple<QString, QString, QString> > list;
    QList<QPair<QString, QString> > list1;
    //if (record) {
        int evalCount=resultsGetEvaluationCount();
        for (int i=0; i<evalCount; i++) {
            QString en=resultsGetEvaluationName(i);
            int jmax=resultsGetCount(en);
            for (int j=0; j<jmax; j++) {
                QString rn=resultsGetResultName(en, j);
                QString lab=resultsGetLabel(en, rn);
                QString labrt=resultsGetLabelRichtext(en, rn);
                if (!l.contains(lab)) {
                    l.append(rn);
                    list.append(qMakeTriple(lab, labrt, rn));
                }
            }
        }
    //}
    if (list.size()>0) {
        qSort(list.begin(), list.end(), QFRawDataRecord_StringTripleCaseInsensitiveCompare);
        for (int i=0; i<list.size(); i++) {
            list1.append(qMakePair(list[i].second, list[i].third));
        }
    }

    return list1;
}*/















QList<QString> QFRawDataRecord::resultsCalcNames(const QString& evalName, const QString& group, const QString& evalgroup) const {
    QStringList l;
    int evalCount=resultsGetEvaluationCount();
    for (int i=0; i<evalCount; i++) {
        QString en=resultsGetEvaluationName(i);
        QString egrp=resultsGetEvaluationGroup(en);
        if ((evalName.isEmpty() || (en==evalName)) && (evalgroup.isEmpty() || (egrp==evalgroup))) {
            int jmax=resultsGetCount(en);
            for (int j=0; j<jmax; j++) {
                QString rn=resultsGetResultName(en, j);
                if ((group.isEmpty() || (group==resultsGetGroup(en, rn))) && (!l.contains(rn))) {
                    l.append(rn);
                }
            }
        }
    }
    if (l.size()>0) l.sort();
    return l;
}


QList<QString> QFRawDataRecord::resultsCalcGroups(const QString& evalName) const {
    QStringList l;
    int evalCount=resultsGetEvaluationCount();
    for (int i=0; i<evalCount; i++) {
        QString en=resultsGetEvaluationName(i);
        if (evalName.isEmpty() || (en==evalName)) {
            int jmax=resultsGetCount(en);
            for (int j=0; j<jmax; j++) {
                QString rn=resultsGetResultName(en, j);
                QString g=resultsGetGroup(en, rn);
                if (!l.contains(g)) {
                    l.append(g);
                }
            }
        }
    }
    if (l.size()>0) l.sort();
    return l;
}


QList<QPair<QString, QString> > QFRawDataRecord::resultsCalcNamesAndLabels(const QString& evalName, const QString& group, const QString& evalgroup) const {
    QStringList l;
    QList<QPair<QString, QString> > list;
    int evalCount=resultsGetEvaluationCount();
    for (int i=0; i<evalCount; i++) {
        QString en=resultsGetEvaluationName(i);
        QString egrp=resultsGetEvaluationGroup(en);
        if ((evalName.isEmpty() || (en==evalName)) && (evalgroup.isEmpty() || (egrp==evalgroup))) {
            int jmax=resultsGetCount(en);
            for (int j=0; j<jmax; j++) {
                QString rn=resultsGetResultName(en, j);
                QString lab=resultsGetLabel(en, rn);
                if ((group.isEmpty() || (group==resultsGetGroup(en, rn))) && (!l.contains(lab))) {
                    l.append(lab);
                    list.append(qMakePair(lab, rn));
                }
            }
        }
    }
    if (list.size()>0) {
        qSort(list.begin(), list.end(), QFRawDataRecord_StringPairCaseInsensitiveCompare);
    }

    return list;
}


QList<QPair<QString, QString> > QFRawDataRecord::resultsCalcNamesAndLabelsRichtext(const QString& evalName, const QString& group, const QString& evalgroup) const {
    QStringList l;
    /*
       This function first creates a list of the triple <label, richtext_label, result_id> that matches the given filters. Then this list
       ist filtered according to the label (it's difficult to sort a rich-texted string!!!). Finally the list is shrunken down to the
       pair <richtext_label, result_id>!
      */
    QList<QTriple<QString, QString, QString> > list;
    QList<QPair<QString, QString> > list1;
    int evalCount=resultsGetEvaluationCount();
    for (int i=0; i<evalCount; i++) {
        QString en=resultsGetEvaluationName(i);
        QString egrp=resultsGetEvaluationGroup(en);
        if ((evalName.isEmpty() || (en==evalName)) && (evalgroup.isEmpty() || (egrp==evalgroup))) {
            int jmax=resultsGetCount(en);
            for (int j=0; j<jmax; j++) {
                QString rn=resultsGetResultName(en, j);
                QString lab=resultsGetLabel(en, rn);
                QString labrt=resultsGetLabelRichtext(en, rn);
                if ((group.isEmpty() || (group==resultsGetGroup(en, rn))) && (!l.contains(lab))) {
                    l.append(lab);
                    list.append(qMakeTriple(lab, labrt, rn));
                }
            }
        }
    }
    if (list.size()>0) {
        qSort(list.begin(), list.end(), QFRawDataRecord_StringTripleCaseInsensitiveCompare);
        for (int i=0; i<list.size(); i++) {
            list1.append(qMakePair(list[i].second, list[i].third));
        }
    }

    return list1;
}


QList<QString> QFRawDataRecord::resultsCalcEvaluationsInGroup(const QString& evalGroup) const {
    QStringList l;

    int evalCount=resultsGetEvaluationCount();
    for (int i=0; i<evalCount; i++) {
        QString en=resultsGetEvaluationName(i);
        if (resultsGetEvaluationGroup(en)==evalGroup) l.append(en);
    }

    return l;
}


QList<QString> QFRawDataRecord::resultsCalcEvalGroups(const QString& paramgroup) const {
    QStringList l;

    int evalCount=resultsGetEvaluationCount();
    for (int i=0; i<evalCount; i++) {
        QString en=resultsGetEvaluationName(i);
        QString grp=resultsGetEvaluationGroup(en);
        if (!l.contains(grp)) {
            if (paramgroup.isEmpty()) {
                l.append(grp);
            } else {
                QList<QString> gl=resultsCalcGroups(en);
                if (gl.contains(paramgroup)) l.append(grp);
            }
        }
    }

    return l;
}


