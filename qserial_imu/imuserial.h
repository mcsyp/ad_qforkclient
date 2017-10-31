#ifndef IMUSERIAL_H
#define IMUSERIAL_H

#include <QSerialPort>
#include "imudecoder.h"

class ImuSerial : public QSerialPort
{
  Q_OBJECT
public:
  static constexpr qint32 MY_BUADRATE=QSerialPort::Baud115200;
  explicit ImuSerial(QObject *parent = 0);

  /* purpose: start the port
   * input:
   *  @portname, the device name of the serial port
   * return:
   *  true if success
   */
  bool begin(const char * portname);

signals:

public slots:
  void onReadyRead();
  void onImuReadyRead();
protected:
  ImuDecoder decoder_;
};

#endif // IMUSERIAL_H
