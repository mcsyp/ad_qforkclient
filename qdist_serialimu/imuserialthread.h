#ifndef IMUSERIAL_H
#define IMUSERIAL_H

#include <QSerialPort>
#include <QThread>
#include <QVector>
#include <QMutex>
#include "imudecoder.h"
#include "dataframe.h"
#include "featuremse.h"

class ImuSerialThread: public QSerialPort
{
  Q_OBJECT
public:
  static constexpr qint32 MY_BAUDRATE=QSerialPort::Baud115200;

  static constexpr int MSEC_ONE_DAY=86400000;
  static constexpr int RECORD_TIMEGAP=50;
  static constexpr int RECORD_LOCK_TIMEOUT=100;//100m

  static constexpr int MSE_RECORD_SIZE=200;
  static constexpr int EULAR_RECORD_SIZE=500;

  static constexpr int GYRO_MSE_SIZE=10;
  static constexpr int GYRO_MSE_COLS=3;
  static constexpr int GYRO_MSE_DELTA=2;
  static constexpr int IMU_AXIS=6;//MUST BE BIGGER THAN GYRO COLS!!!

  typedef struct imu_record_s{
    int ts;
    float axis[IMU_AXIS];
  }imu_record_t;
  typedef QVector<imu_record_t> ImuRecordList;

  explicit ImuSerialThread(QObject *parent = 0   );

  /* purpose: start the port
   * input:
   *  @portname, the device name of the serial port
   * return:
   *  true if success
   */
  bool begin(QString portname);

  bool searchRecord(int ts,ImuRecordList & list,imu_record_t * out_record);

signals:
  void postBegin(QString portname);
  void imuRecordUpdate(int ts);

public slots:
  void onReadyRead();
  void onImuReadyRead();

protected slots:
  void onBegin(QString portname);

protected:
  ImuDecoder decoder_;
  QThread worker_thread_;

  QMutex record_lock_;
  qri_neuron_lib::DataFrame * ptr_gyro_frame_;
  qri_neuron_lib::FeatureMSE feature_mse_;
public:
  ImuRecordList eular_list_;
  ImuRecordList mse_list_;
};

#endif // IMUSERIAL_H
