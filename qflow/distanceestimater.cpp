#include "distanceestimater.h"
#include <qdebug.h>
#include <qmath.h>

DistanceEstimater::DistanceEstimater(QObject *parent) : QObject(parent)
{
  connect(&laser_timer_,SIGNAL(timeout()),this,SLOT(onLaserTimeout()));
  connect(&laser_,SIGNAL(distanceChanged(uint64_t,float)),this,SLOT(onGroundDistanceChanged(uint64_t,float)));
  connect(&flow_,SIGNAL(flowChanged(uint64_t,float,float)),this,SLOT(onFlowChanged(uint64_t,float,float)));

  this->ground_distance_ = 0.0f;
  this->setLensFocal(LENS_FOCAL_LEN);
}

bool DistanceEstimater::begin(){
  //step1.begin serial devs
  if(!laser_.begin(DE_SERIAL_LASER_DEV)){
    qDebug()<<tr("[%1,%2]fail to opend the laser serial:%3").arg(__FILE__).arg(__LINE__).arg(DE_SERIAL_LASER_DEV);
    goto FAIL;
  }
  if(!flow_.begin(DE_SERIAL_FLOW_DEV)){
    qDebug()<<tr("[%1,%2]fail to opend the pix4flow serial:%3").arg(__FILE__).arg(__LINE__).arg(DE_SERIAL_FLOW_DEV);
    goto FAIL;
  }

  //step2.reset flow
  resetMath();

  //step3.begin laser timer
  laser_timer_.start(LASER_UPDATE_TIMEOUT);

  return true;
FAIL:
  stop();
  return false;
}

void DistanceEstimater::stop(){
  laser_timer_.stop();
  laser_.close();
  flow_.close();

  resetMath();
}

void DistanceEstimater::resetMath(){
  //reset accumulated
  this->accumulated_x_ = 0.0f;
  this->accumulated_y_ = 0.0f;
  this->accumulated_dist_x_ = 0.0f;
  this->accumulated_dist_y_ = 0.0f;

  //reset ground distance
  this->ground_distance_ = 0.0f;
  this->ground_trgger_time_ = 0;
}

void DistanceEstimater::setLensFocal(float lens)
{
  if(lens<=0.0f)return;
  this->lens_focal_len_ = lens;
  this->focal_length_pix_ =   (lens/1000.0f)/(SIZE_PER_PIXEL/1000000.0f);
}

void DistanceEstimater::onLaserTimeout(){
  //step2.trigger time
  laser_.trigger();
  ground_trgger_time_ = QTime::currentTime().msecsSinceStartOfDay();
}

void DistanceEstimater::onGroundDistanceChanged(uint64_t timestamp, float dist)
{
  (void)timestamp;
  ground_distance_ = dist;//update the ground distance
}

void DistanceEstimater::onFlowChanged(uint64_t timestamp, float dx, float dy)
{
  (void)timestamp;
  if(fabs(dx)<=0.2 && fabs(dy)<=0.2)return;

  accumulated_y_ += dy;
  printf("flow:%.2f, accumulated:%.2f,",dy,accumulated_y_);

  if(ground_distance_){
    float dist_y = ground_distance_*dy/this->focal_length_pix_;
    float vel_y = dist_y*1000.0f/flow_.msecBetweenFrames();//velocity in m/s
    accumulated_dist_y_ += dist_y;
    printf("ground_dist:%.2f,velocity:%.2fm/s, dist:%.2fm",ground_distance_,vel_y,accumulated_dist_y_);
  }
  printf("\n");

}
