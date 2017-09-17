#ifndef SERIALSLAVE_H
#define SERIALSLAVE_H

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTime>
#include <QTimer>
#include "myhdlc.h"
#include "servercmdid.h"

class SerialBle : public QSerialPort
{
  Q_OBJECT
public:
  static constexpr qint32 MY_BUADRATE=QSerialPort::Baud9600;
  static constexpr qint64 LAST_RX_TIMEOUT=15000;//15sec
  static constexpr qint64 STATE_CHECK_TIMEOUT=10000;//10sec

  explicit SerialBle(QObject *parent = 0);

  /*purpose: begin this serial port to connect to slave ble
   *input:
   *@portname:the port's name, eg:ttyS1
   *return:
   * true if the port is open successfully
   */
  bool Begin(const QString portname);

  void HostBle(bool b);
  void ConnectToBle(QString mac);
  void ResetBle();

  server_protocol_cmd::ble_upload_t SlaveInfo(){return slave_info_;}
signals:
  void bleRxDataReady();
  void bleDisconnected();

public slots:
  void onReadyRead();
  void onHdlcFrameDecoded(char* frame, int size);
  void onHBTimeout();
protected:
  MyHdlc hdlc_;

private:
  server_protocol_cmd::ble_upload_t slave_info_;
  qint64 last_rx_time_;
  QTimer rx_hb_timer_;
};

#endif // SERIALLASER_H
