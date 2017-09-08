#ifndef SERIALSLAVE_H
#define SERIALSLAVE_H

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTime>
#include "myhdlc.h"

class SerialSlave : public QSerialPort
{
  Q_OBJECT
public:
#define SLAVE_BAUDRATE QSerialPort::Baud9600

  explicit SerialSlave(QObject *parent = 0);

  /*purpose: begin this serial port to connect to PIX4FLOW
   *input:
   *@portname:the port's name, eg:ttyS1
   *return:
   * true if the port is open successfully
   */
  bool begin(const QString portname);
signals:
  void distanceChanged(int t,float distance);

public slots:
  void onReadyRead();

  void onHdlcFrameEncoded(QByteArray data);
  void onHdlcFrameDecoded(QByteArray receive_frame_buffer, quint16 frame_size);
protected:
  hdlc_qt::MyHdlc hdlc_;
};

#endif // SERIALLASER_H
