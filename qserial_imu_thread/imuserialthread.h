#ifndef IMUSERIAL_H
#define IMUSERIAL_H

#include <QSerialPort>
#include <QThread>
#include "imudecoder.h"

class ImuSerialThread: public QSerialPort
{
  Q_OBJECT
public:
  static constexpr qint32 MY_BAUDRATE=QSerialPort::Baud115200;
  explicit ImuSerialThread(QObject *parent = 0);

  /* purpose: start the port
   * input:
   *  @portname, the device name of the serial port
   * return:
   *  true if success
   */
  bool begin(const char * portname);

signals:
  void postBegin(QString portname);

public slots:
  void onReadyRead();
  void onImuReadyRead();

protected slots:
  void onBegin(QString portname);

protected:
  ImuDecoder decoder_;
  QThread worker_thread_;
};

#endif // IMUSERIAL_H
