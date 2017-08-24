#ifndef DISTANCEESTIMATER_H
#define DISTANCEESTIMATER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QMutex>

#include "seriallaser.h"
#include "serialpix4flow.h"
#include "configparser.h"
#include "dmpthread.h"

#define DEFAULT_SERIAL_LASER_DEV "ttyS2"
#define DEFAULT_SERIAL_FLOW_DEV "ttyS1"

#define CONFIG_KEY_SERIAL_LASER         "dev_serial_laser"
#define CONFIG_KEY_SERIAL_FLOW          "dev_serial_flow"
#define CONFIG_KEY_LENS_FOCAL           "lens_focal_len"
#define CONFIG_KEY_LASER_UPDATE_TIMEOUT "laser_update_timeout"
#define CONFIG_KEY_DMP_UPDATE_TIMEOUT   "dmp_update_timeout"
#define CONFIG_KEY_DMP_RECORD_SIZE      "dmp_record_size"
#define CONFIG_KEY_DMP_MAX_TIMEGAP      "dmp_max_timegap"

class DistanceEstimater : public QObject
{
  Q_OBJECT
public:
  static constexpr float LENS_FOCAL_LEN = 4.0f;//in mm
  static constexpr float SIZE_PER_PIXEL = 24.0f;//24 micro meter/pixel
  static constexpr uint64_t LASER_UPDATE_TIMEOUT=15000;//in miliseconds

  explicit DistanceEstimater(QObject *parent = 0);

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

  //stop the estimater
  void stop();

  //reset the accumulated metters and pixels
  void clearEstimater();

  //set the lens focal size in (mm)
  void setLensFocal(float lens);
public slots:
  void onGroundDistanceChanged(int ts,float dist);//update the ground distance in meter
  void onFlowChanged(int ts,float dx,float dy);
  void onLaserTimeout();

protected:
  SerialLaser laser_;
  SerialPix4Flow flow_;
  DMPThread dmp_source_;
  QTimer laser_timer_;

  uint64_t ground_trgger_time_;
  float ground_distance_;

  float lens_focal_len_;
  float focal_length_pix_;

  float accumulated_x_;
  float accumulated_y_;
  float accumulated_dist_x_;
  float accumulated_dist_y_;
};

#endif // DISTANCEESTIMATER_H
