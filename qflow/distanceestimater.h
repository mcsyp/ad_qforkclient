#ifndef DISTANCEESTIMATER_H
#define DISTANCEESTIMATER_H

#include <QObject>
#include <QTimer>

#include "seriallaser.h"
#include "serialpix4flow.h"

#define DE_SERIAL_LASER_DEV "ttyS2"
#define DE_SERIAL_FLOW_DEV "ttyS1"
class DistanceEstimater : public QObject
{
  Q_OBJECT
public:
  static constexpr float LENS_FOCAL_LEN = 4.0f;//in mm
  static constexpr float SIZE_PER_PIXEL = 24.0f;//24 micro meter/pixel


  static constexpr uint64_t LASER_UPDATE_TIMEOUT=15000;//in miliseconds
  explicit DistanceEstimater(QObject *parent = 0);
  bool begin();
  void stop();

  void resetMath();

  void setLensFocal(float lens);
signals:

public slots:
  void onGroundDistanceChanged(uint64_t timestamp,float dist);//update the ground distance in meter
  void onFlowChanged(uint64_t timestamp,float dx,float dy);
  void onLaserTimeout();

protected:
  SerialLaser laser_;
  SerialPix4Flow flow_;
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
