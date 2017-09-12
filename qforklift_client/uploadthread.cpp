#include "uploadthread.h"
#include "servercmdid.h"
#include "serverprotocol.h"
#include <qdebug.h>
#include <QFile>
#include <QTextStream>

using namespace server_protocol_cmd;
UploadThread::UploadThread(QObject *parent) : QThread(parent){
  ptr_socket_ = NULL;
}

void UploadThread::UploadFile(QString path, QTcpSocket *socket)
{
  upload_path_ = path;
  ptr_socket_  = socket;
  start();
}
void UploadThread::run(){
  if(ptr_socket_==NULL)return;

  const int head_size=32;
  uint8_t head_buffer[head_size];
  int data_size=0;

  QFile file(upload_path_);
  if(!file.open(QIODevice::ReadOnly)){
    qDebug()<<tr("[%1,%2]fail to open %3").arg(__FILE__).arg(__LINE__).arg(upload_path_);
    return;
  }

  int head_len = ServerProtocol::FillHead(SERVER_UPLOAD_REQ,file.size(),head_buffer,head_size);
  ptr_socket_->write((char*)head_buffer,head_len);

  QTextStream stream(&file);
  while(!stream.atEnd()){
    QString data = stream.read(256);
    ptr_socket_->write(data.toLatin1().data(),data.length());
    data_size+=data.size();
  }
  file.close();
  qDebug()<<tr("[%1,%2]uploaded file[%3/%4]").arg(__FILE__).arg(__LINE__).arg(data_size).arg(file.size());
}


