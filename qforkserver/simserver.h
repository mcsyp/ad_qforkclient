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
  static constexpr int SERVER_PORT=7000;

  explicit SimServer(QObject *parent = 0);

  void StartService(int port);

  void SendBytes(uint16_t cmd_id, uint8_t * data, uint16_t len);

protected slots:
  void onNewConnection();

  void onClientReadyRead();
  void onClientDisconnected();

  void onPayloadReady(int cmdid,QByteArray payload);
protected:
  QTcpSocket* ptr_soket_;
  ServerProtocol protocol_;
};

#endif // SIMSERVER_H
