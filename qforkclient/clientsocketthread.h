#ifndef CLIENTSOCKETTHREAD_H
#define CLIENTSOCKETTHREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QByteArray>
#include "configparser.h"

#include "serverprotocol.h"

#define CONFIG_KEY_SERVER_ADDRESS            "server_address"
#define CONFIG_KEY_SERVER_PORT               "server_port"
#define CONFIG_KEY_SERVER_HB_TIMEOUT         "server_hb_timeout"

#define CONFIG_KEY_DEVID                     "devid"

class ClientSocketThread : public QTcpSocket
{
  Q_OBJECT
public:
  static constexpr int SERVER_HB_TIMEOUT=32000;//30sec

  explicit ClientSocketThread(QObject *parent = 0);
  bool Begin(ConfigParser::ConfigMap& configs);

public slots:
  void onConnected();
  void onDisconnected();

  void onReadyRead();
  void onPayloadReady(int cmdid,QByteArray array);

  void onReconnectTimeout();
  void onUploadTimeout(QString file_path);
protected:
  QThread event_thread_;
  QString server_address_;
  ServerProtocol protocol_;
  int server_port_;

  int hb_ts_;
  int hb_timeout_;

  QString upload_path_;

  QString devid_;
public:
  int server_time_;
  int local_time_offset_;

};

#endif // CLIENTSOCKETTHREAD_H
