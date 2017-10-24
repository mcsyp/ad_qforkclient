#ifndef FORKCLIENT_H
#define FORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>
#include <QThread>
#include <QFile>

#include "configparser.h"
#include "serverprotocol.h"
#include "distanceestimater.h"
#include "serialble.h"
#include "clientsocketthread.h"
#include "mytask.h"

#define CONFIG_KEY_SERVER_RECONNECT_TIMEOUT  "server_reconnect_timeout"
#define CONFIG_KEY_SERVER_UPLOAD_TIMEOUT     "server_upload_timeout"

#define CONFIG_KEY_RECORD_UPLOAD_PATH        "record_upload"
#define CONFIG_KEY_RECORD_TEMP_PATH          "record_temp"
#define CONFIG_KEY_RECORD_TIMEOUT            "record_timeout"

#define CONFIG_KEY_SLAVE_BLE                 "slave_ble"
#define CONFIG_KEY_BLE_SERIAL                "dev_serial_ble"
#define CONFIG_KEY_MOVE_STATE_THRESHOLD      "move_state_threshold"

class ForkliftClient : public QObject
{
  Q_OBJECT
public:
  static constexpr int SERVER_RECONNECTION_TIMEOUT=5000;//reconnect every 5 secs
  static constexpr int SERVER_UPLOAD_TIMEOUT=61000;//60sec
  static constexpr int RECORD_TIMEOUT=1000;//
  static constexpr int TASK_TIMEOUT=1000;//
  static constexpr int TASK_TRIGGER=200;//200ms
  static constexpr float MOVE_STATE_THRESHOLD=0.01;//1cm
  static constexpr int GPIO_EN_PIX4=67;
  static constexpr int GPIO_EN_LASER=66;
  static constexpr int GPIO_BLE_STATE=65;

  explicit ForkliftClient(QObject *parent = nullptr);
  ~ForkliftClient();

  bool Begin(ConfigParser::ConfigMap & configs);

signals:
  void uploadRecordReady(QString file_path);

public slots:
  void onSlaveBleReady();
  void onSlaveBleDisconnected();

  void onTaskTimeout();
  void onTaskRecordTimeout(int ms);
  void onTaskUploadTimeout(int ms);
public:
  /*device settings*/
  SerialBle slave_ble_;
  DistanceEstimater dist_estimater_;

private:
  /*server settings*/
  ClientSocketThread client_socket_;
  int server_reconnect_timeout_;

  //local parameters
  QString devid_;
  QString slave_mac_;

  server_protocol_cmd::ble_upload_t ble_info_;
  //timers
  QTimer reconnect_timer_;
  QTimer task_timer_;

  QFile record_tmp_file_;
  QTextStream record_tmp_text_;
  QString record_upload_path_;
  /*
  int task_record_timeout_;
  int task_record_ts_;

  int task_upload_timeout_;//time to upload the data
  int task_upload_ts_;
  */
  MyTask task_record_;
  MyTask task_upload_;

  float move_state_threshold_;
};

#endif // FORKCLIENT_H
