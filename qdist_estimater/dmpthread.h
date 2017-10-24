#ifndef DMPTHREAD_H
#define DMPTHREAD_H

#include <QThread>
#include <QVector>
#include <QMutex>
#include "helper_3dmath.h"
#include "dataframe.h"
#include "featuremse.h"
class DMPThread: public QThread
{
  Q_OBJECT
public:
  static constexpr int DEFAULT_UPDATE_TIMEOUT =5;
  static constexpr int DEFAULT_RECORD_SIZE=500;
  static constexpr int DEFAULT_MAX_TIMEGAP=50;

  static constexpr int RECORD_UPDATE_LOCK_TIMEOUT=200;//100ms
  static constexpr int MSEC_ONE_DAY=86400000;
  static constexpr float MAX_RANGE = 32767.0f;
  static constexpr int GYRO_MSE_SIZE=10;
  static constexpr int GYRO_MSE_COLS=3;
  static constexpr int GYRO_MSE_DELTA=2;

  typedef struct imu_record_s{
    int ts;//timestamp
    float ypr[3];//yaw, pitch, roll
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
    float gyro_mse[GYRO_MSE_COLS];
  }imu_record_t;
  typedef QVector<imu_record_t> ImuRecordList;

  explicit DMPThread(QObject * parent=NULL);

  void StartDmpThread(int timeout, int record_size, int timegap=DEFAULT_MAX_TIMEGAP);

  bool SyncRecord(int t,imu_record_t & record);

  float ConvertAccel(int16_t a);
  float ConvertGyro(int16_t g);
signals:
  void dmpUpdated(int ts);
protected:
  void run();

protected:
  int update_timeout_;
  int record_size_;
  QMutex record_mutex_;
  int record_max_timegap_;

  float accel_range_;
  float gyro_range_;

  qri_neuron_lib::FeatureMSE mse_;
  qri_neuron_lib::DataFrame * ptr_dataframe_;
public:
  ImuRecordList record_list_;
};

#endif // DMPTHREAD_H
