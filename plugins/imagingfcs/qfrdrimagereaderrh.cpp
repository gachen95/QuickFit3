#include "qfrdrimagereaderrh.h"

#include <QObject>
#include <QtGlobal>

QFRDRImageReaderRH::QFRDRImageReaderRH() {
    width=0;
    height=0;
    filename="";
    frameSize=0;
    file=NULL;
}

QFRDRImageReaderRH::~QFRDRImageReaderRH() {
    if (file) close();
}

QString QFRDRImageReaderRH::filter() const {
    return QObject::tr("raw RH file (*.rrh *.dat)");
}

QString QFRDRImageReaderRH::formatName() const {
    return QObject::tr("RadHard raw Ddata");
}


unsigned int QFRDRImageReaderRH::calculateFrameSize() {
  // lets assume we have correct shapes frames here... with correct CRCs
  //only accepts 32 bit packed data format

  unsigned char *buffer = new unsigned char [MAX_FRAME_SIZE];
  QDataStream in(file);
  unsigned int len = in.readRawData((char*)buffer, MAX_FRAME_SIZE);

  unsigned char *bufferPos=buffer;
  unsigned int pos=0;
  unsigned frameSize=0;
  file->seek(0);
  uint16_t crc=0xFFFFU;
  do {
    crc=crc_ccitt(crc,bufferPos,4);
    bufferPos+=4;
    pos+=4;
    uint16_t crc0=*((uint16_t*)bufferPos);
    if(crc0==crc)frameSize=pos;
  }while((pos<len)||(frameSize!=0));
  file->seek(0);
  return frameSize;
}


bool QFRDRImageReaderRH::open(QString filename) {
  close();
  bool result=true;
  file = new QFile(filename);
  if (!file->open(QIODevice::ReadOnly))
    return false;
  frameSize=calculateFrameSize();
  switch(frameSize) {
    case  134: width= 32; height= 32; break;
    case 2058: width=128; height=128; break;
    default: frameSize=0; break;
  }
  if(width==0||height==0) {
    close();
    result=false;
  }
  return result;
}

void QFRDRImageReaderRH::close() {
  if (!file) return ;
  file->close();
  delete file;
  file=NULL;
}

uint32_t QFRDRImageReaderRH::countFrames() {
  uint32_t result=0;
  if (!file) return 0;
  unsigned int size=file->size();
  if(frameSize!=0) {
    if((size % frameSize)==0)
      result=size/frameSize;
  }
  return result;
}

bool QFRDRImageReaderRH::nextFrame() {
    if (!file) return false;
    return not file->atEnd();
}

void QFRDRImageReaderRH::reset() {
    if (!file) return ;
    file->seek(0);
}

uint16_t QFRDRImageReaderRH::frameWidth() {
    return width;
}

uint16_t QFRDRImageReaderRH::frameHeight() {
    return height;
}

bool QFRDRImageReaderRH::readFrameUINT16(uint16_t* data) {
  if (!file) return false;
  return readFrame_<uint16_t>(data);
}

bool QFRDRImageReaderRH::readFrameFloat(float* data) {
    if (!file) return false;
  return readFrame_<float>(data);
}