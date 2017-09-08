#ifndef SIMSERVER_H
#define SIMSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QByteArrayList>

#include "serverprotocol.h"


class SimServer : public QTcpServer
{
  Q_OBJECT
public:
  static constexpr int SERVER_PORT=6000;
  explicit SimServer(QObject *parent = 0);

  void StartService(int port);

  void SendBytes(uint16_t cmd_id, uint8_t * data, uint16_t len);
protected:
  enum SERVER_CMDID {
    SERVER_TIME_REQ=0x1,
    SERVER_TIME_ACK=0x2,
    SERVER_FILE_REQ=0x3,
    SERVER_FILE_ACK=0x4,
    SERVER_HEART_REQ=0x5,
    SERVER_HEART_ACK=0x6,
    SERVER_RECORD_REQ=0x7,
    SERVER_RECORD_ACK=0x8,
    SERVER_MASTER_BATTERY_REQ=0x9,
    SERVER_SLAVE_BATTERY_REQ=0xA,
    SERVER_LOG_REQ=0xAA,
  };

protected slots:
  void onNewConnection();

  void onClientReadyRead();
  void onClientDisconnected();

  void onPayloadReady(int cmdid,QByteArray& payload);
protected:
  QTcpSocket* ptr_soket_;
  ServerProtocol protocol_;
};

#endif // SIMSERVER_H
