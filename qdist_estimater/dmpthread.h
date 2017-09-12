#ifndef DMPTHREAD_H
#define DMPTHREAD_H

#include <QThread>
#include <QVector>
#include <QMutex>
#include "helper_3dmath.h"
class DMPThread: public QThread
{
  Q_OBJECT
public:
  typedef struct imu_record_s{
    int ts;//timestamp
    float ypr[3];//yaw, pitch, roll
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
  }imu_record_t;
  typedef QVector<imu_record_t> ImuRecordList;

  static constexpr int DEFAULT_UPDATE_TIMEOUT =5;
  static constexpr int DEFAULT_RECORD_SIZE=500;
  static constexpr int DEFAULT_MAX_TIMEGAP=50;

  static constexpr int RECORD_UPDATE_LOCK_TIMEOUT=200;//100ms
  static constexpr int MSEC_ONE_DAY=86400000;
  static constexpr float MAX_RANGE = 32767.0f;


  explicit DMPThread(QObject * parent=NULL);

  void StartDmpThread(int timeout, int record_size, int timegap=DEFAULT_MAX_TIMEGAP);

  bool SyncRecord(int t,imu_record_t & record);

  float ConvertAccel(int16_t a);
  float ConvertGyro(int16_t g);
protected:
  void run();

protected:
  int update_timeout_;
  int record_size_;
  QMutex record_mutex_;
  int record_max_timegap_;

  float accel_range_;
  float gyro_range_;
public:
  ImuRecordList record_list_;
};

#endif // DMPTHREAD_H
