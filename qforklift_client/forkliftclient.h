#ifndef FORKCLIENT_H
#define FORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>
#include <QFile>

#include "configparser.h"
#include "serverprotocol.h"
#include "distanceestimater.h"
#include "serialble.h"
#include "uploadthread.h"

#define CONFIG_KEY_SERVER_ADDRESS            "server_address"
#define CONFIG_KEY_SERVER_PORT               "server_port"
#define CONFIG_KEY_SERVER_RECONNECT_TIMEOUT  "server_reconnect"
#define CONFIG_KEY_SERVER_UPLOAD_TIMEOUT     "server_upload_timeout"
#define CONFIG_KEY_SERVER_HB_TIMEOUT         "server_hb_timeout"

#define CONFIG_KEY_RECORD_TEMP               "record_temp"
#define CONFIG_KEY_RECORD_UPLOAD             "record_upload"
#define CONFIG_KEY_RECORD_TIMEOUT            "record_timeout"

#define CONFIG_KEY_SLAVE_BLE                 "slave_ble"
#define CONFIG_KEY_DEVID                     "devid"
#define CONFGI_KEY_BLE_SERIAL                "dev_serial_ble"

#define RECORD_UPLOAD_PATH "./record_upload.csv"
class ForkliftClient : public QTcpSocket
{
  Q_OBJECT
public:
  static constexpr int SERVER_RECONNECTION_TIMEOUT=5000;//reconnect every 5 secs
  static constexpr int SERVER_UPLOAD_TIMEOUT=61000;//60sec
  static constexpr int SERVER_HB_TIMEOUT=32000;//30sec
  static constexpr int RECORD_TIMEOUT=1000;//

  explicit ForkliftClient(QObject *parent = nullptr);
  ~ForkliftClient();

  bool Begin(ConfigParser::ConfigMap & configs);
protected:
  void UploadFile(QString path);
public slots:

  void onConnected();
  void onDisconnected();
  void onReadyRead();

  void onReconnectTimeout();
  void onUploadTimeout();
  void onHBTimeout();
  void onRecordTimeout();

  void onTrajectoryUpdated();

  void onPayloadReady(int cmdid,QByteArray payload);

  void onSlaveBleReady();
  void onSlaveBleDisconnected();

public:
  /*device settings*/
  SerialBle slave_ble_;
  DistanceEstimater dist_estimater_;
  server_protocol_cmd::trajectory_point_t dist_track_;

private:
  /*server settings*/
  ServerProtocol protocol_;
  QString server_address_;
  int server_port_;
  int server_retry_timeout_;

  //local parameters
  QString devid_;
  QString slave_mac_;
  int upload_timeout_;//time to upload the data
  int hb_timeout_;//time to sync the hb to server
  server_protocol_cmd::ble_upload_t ble_info_;
  //server time
  qint64 server_time_;
  qint64 local_time_offset_;

  //timers
  QTimer reconnect_timer_;
  QTimer upload_timer_;
  QTimer hb_timer_;
  QTimer record_timer_;

  QFile record_tmp_file_;
  QTextStream record_tmp_text_;
  int record_timeout_;
};

#endif // FORKCLIENT_H
