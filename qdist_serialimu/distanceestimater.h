#ifndef DISTANCEESTIMATER_H
#define DISTANCEESTIMATER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QByteArray>

#include "seriallaser.h"
#include "serialpix4flow.h"
#include "configparser.h"
#include "imuserialthread.h"
#include "dataframe.h"
#include "featuremse.h"

#define DEFAULT_SERIAL_LASER_DEV "ttyS2"
#define DEFAULT_SERIAL_FLOW_DEV "ttyS1"
#define DEFAULT_SERIAL_IMU "ttyUSB0"

#define CONFIG_KEY_SERIAL_LASER         "dev_serial_laser"
#define CONFIG_KEY_SERIAL_FLOW          "dev_serial_flow"
#define CONFIG_KEY_SERIAL_IMU           "dev_serial_imu"

#define CONFIG_KEY_LENS_FOCAL           "lens_focal_len"
#define CONFIG_KEY_LASER_UPDATE_TIMEOUT "laser_update_timeout"

#define CONFIG_KEY_DMP_UPDATE_TIMEOUT   "dmp_update_timeout"
#define CONFIG_KEY_DMP_RECORD_SIZE      "dmp_record_size"
#define CONFIG_KEY_DMP_MAX_TIMEGAP      "dmp_max_timegap"

#define CONFIG_KEY_GYRO_FRAME_SIZE      "gyro_frame_size"
#define CONFIG_KEY_GYRO_MSE_THRESHOLD_X   "gyro_mse_threshold_x"
#define CONFIG_KEY_GYRO_MSE_THRESHOLD_Y   "gyro_mse_threshold_y"
#define CONFIG_KEY_GYRO_MSE_TIMETOUT     "gyro_mse_timeout"

#define CONFIG_KEY_LASER_MIN_DIST       "laser_min_dist"
#define CONFIG_KEY_LASER_MAX_DIST       "laser_max_dist"

#define CONFIG_KEY_MIN_FLOW_X "flow_threshold_x"
#define CONFIG_KEY_MIN_FLOW_Y "flow_threshold_y"

class DistanceEstimater : public QObject
{
  Q_OBJECT
public:
  static constexpr float LENS_FOCAL_LEN = 16.0f;//in mm
  static constexpr float SIZE_PER_PIXEL = 24.0f;//24 micro meter/pixel

  static constexpr uint64_t LASER_UPDATE_TIMEOUT=15000;//in miliseconds

  static constexpr float GYRO_MSE_THRESHOLD=0.5;
  static constexpr int   GYRO_MSE_TIMEOUT=2000;//2sec

  static constexpr float FLOW_MIN_THRESHOLD_X=0.5f;
  static constexpr float FLOW_MIN_THRESHOLD_Y=0.5f;

   explicit DistanceEstimater(QObject *parent = 0);
  ~DistanceEstimater();

  /*purpose: begin the distance esitmater by default settings
   *return: true if success
   */
  bool begin();

  /*purpose: begin the distance esitmater by default settings
   *input:
   * @configs, the global settings from the local config file
   *return: true if success
   */
  bool begin(const ConfigParser::ConfigMap & configs);

  //reset the accumulated metters and pixels
  void reset();

  //set the lens focal size in (mm)
  void setLensFocal(float lens);

  //result access functions
  float velocity()const {return velocity_;}
  void  distanceDelta(float *x, float *y);
  float distance()const {return distance_;}
  float distanceAbs()const{return distance_abs_;}
  float heading()const {return heading_;}
  int lastUpdateTimestamp() const {return update_timestamp_;}
protected:
  bool validateMSE(int ts);
signals:
  void trajectoryUpdated();

public slots:
  void onLaserUpdated(int ts,float dist);//update the ground distance in meter
  void onFlowChanged(int ts,float dx,float dy);
  void onImuRecordUpdated(int ts);

protected:
  SerialLaser serial_laser_;
  SerialPix4Flow serial_flow_;
  ImuSerialThread serial_imu_;

  //laser related
  int laser_update_timeout_;
  int laser_update_ts_;
  float laser_distance_;

  //focal lens realted
  float focal_len_;
  float focal_length_pix_; 
  float flow_min_threshold_x_;
  float flow_min_threshold_y_;

  //mse check related members
  float gyro_mse_threshold_x_;
  float gyro_mse_threshold_y_;
  int gyro_mse_pass_ts_;
  int gyro_mse_timeout_;

  //record related
  float delta_x_;
  float delta_y_;
  float distance_;
  float distance_abs_;
  float velocity_;
  float heading_;
  int   update_timestamp_;

};

#endif // DISTANCEESTIMATER_H
