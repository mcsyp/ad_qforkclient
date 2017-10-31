#include "distanceestimater.h"
#include <qdebug.h>
#include <qmath.h>

using namespace qri_neuron_lib;
DistanceEstimater::DistanceEstimater(QObject *parent) : QObject(parent)
{
  connect(&serial_laser_,SIGNAL(distanceChanged(int,float)),this,SLOT(onLaserUpdated(int,float)));
  connect(&serial_flow_,SIGNAL(flowChanged(int,float,float)),this,SLOT(onFlowChanged(int,float,float)));
  connect(&serial_imu_,SIGNAL(imuRecordUpdate(int)),this,SLOT(onImuRecordUpdated(int)));

  this->laser_distance_ = 0.0f;
  this->setLensFocal(LENS_FOCAL_LEN);
}

DistanceEstimater::~DistanceEstimater(){
}

bool DistanceEstimater::begin(){
  ConfigParser::ConfigMap map;
  map.clear();
  return begin(map);
}

bool DistanceEstimater::begin(const ConfigParser::ConfigMap &configs)
{
  //step1.begin serial devs
  do{
    QString dev = DEFAULT_SERIAL_LASER_DEV;
    if(configs.contains(CONFIG_KEY_SERIAL_LASER)){
      dev = configs[CONFIG_KEY_SERIAL_LASER];
    }
    if(!serial_laser_.begin(dev)){
      qDebug()<<tr("[%1,%2]fail to opend the laser serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
      return false;
    }
    qDebug()<<tr("[%1,%2]laser serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
  }while(0);

  do{//flow serial
    QString dev = CONFIG_KEY_SERIAL_FLOW;
    if(configs.contains(CONFIG_KEY_SERIAL_FLOW)){
      dev = configs[CONFIG_KEY_SERIAL_FLOW];
    }
    if(!serial_flow_.begin(dev)){
      qDebug()<<tr("[%1,%2]fail to opend the flow serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
      return false;
    }
    qDebug()<<tr("[%1,%2]flow serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
  }while(0);

  do{//serial imu
    QString dev = DEFAULT_SERIAL_IMU;
    if(configs.contains(CONFIG_KEY_SERIAL_IMU)){
      dev = configs[CONFIG_KEY_SERIAL_IMU];
    }
    if(!serial_imu_.begin(dev.toLatin1().data())){
      qDebug()<<tr("[%1,%2]fail to opend the imu serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
      return false;
    }
    qDebug()<<tr("[%1,%2]imu serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
  }while(0);

  do{//set lens focal len
    float lens = LENS_FOCAL_LEN;
    if(configs.contains(CONFIG_KEY_LENS_FOCAL)){
      lens = configs[CONFIG_KEY_LENS_FOCAL].toFloat();
    }
    setLensFocal(lens);
    qDebug()<<tr("[%1,%2]focal_lens:%3mm, focal_length_pix:%4pix").arg(__FILE__).arg(__LINE__).arg(this->focal_len_).arg(this->focal_length_pix_);
  }while(0);

  do{//start laserfocal_len
    laser_update_timeout_ = LASER_UPDATE_TIMEOUT;
    if(configs.contains(CONFIG_KEY_LASER_UPDATE_TIMEOUT)){
      laser_update_timeout_ = configs[CONFIG_KEY_LASER_UPDATE_TIMEOUT].toLong();
    }
    //laser_timer_.start(laser_update_timeout);
    qDebug()<<tr("[%1,%2]laser_update_timeout:%3").arg(__FILE__).arg(__LINE__).arg(laser_update_timeout_);
  }while(0);

  do{//load gyro viberation info
    gyro_mse_threshold_x_ = GYRO_MSE_THRESHOLD;
    gyro_mse_threshold_y_ = GYRO_MSE_THRESHOLD;
    if(configs.contains(CONFIG_KEY_GYRO_MSE_THRESHOLD_X)){
      gyro_mse_threshold_x_ = configs[CONFIG_KEY_GYRO_MSE_THRESHOLD_X].toFloat();
    }
    if(configs.contains(CONFIG_KEY_GYRO_MSE_THRESHOLD_Y)){
      gyro_mse_threshold_y_ = configs[CONFIG_KEY_GYRO_MSE_THRESHOLD_Y].toFloat();
    }

    gyro_mse_timeout_  = GYRO_MSE_TIMEOUT;
    if(configs.contains(CONFIG_KEY_GYRO_MSE_TIMETOUT)){
      gyro_mse_timeout_ = configs[CONFIG_KEY_GYRO_MSE_TIMETOUT].toInt();
    }
    gyro_mse_pass_ts_ =  0;

    qDebug()<<tr("[%1,%2]gyro queue_size:%3,mse_threshold:(%4,%5),mse_timeout:%7ms")
              .arg(__FILE__).arg(__LINE__)
              .arg(gyro_mse_threshold_x_)
              .arg(gyro_mse_threshold_y_)
              .arg(gyro_mse_timeout_);
  }while(0);

  do{
    this->flow_min_threshold_x_ = FLOW_MIN_THRESHOLD_X;
    this->flow_min_threshold_y_ = FLOW_MIN_THRESHOLD_Y;
    if(configs.contains(CONFIG_KEY_MIN_FLOW_X)){
      this->flow_min_threshold_x_ = configs[CONFIG_KEY_MIN_FLOW_X].toFloat();
    }
    if(configs.contains(CONFIG_KEY_MIN_FLOW_Y)){
      this->flow_min_threshold_y_ = configs[CONFIG_KEY_MIN_FLOW_Y].toFloat();
    }
    qDebug()<<tr("[%1,%2]min_flow_x:%3,min_flow_y:%4")
              .arg(__FILE__).arg(__LINE__)
              .arg(this->flow_min_threshold_x_)
              .arg(this->flow_min_threshold_y_);
  }while(0);


  do{//Load Laser distance information
    if(configs.contains(CONFIG_KEY_LASER_MIN_DIST) &&
       configs.contains(CONFIG_KEY_LASER_MAX_DIST)){
      float min_dist = configs[CONFIG_KEY_LASER_MIN_DIST].toFloat();
      float max_dist = configs[CONFIG_KEY_LASER_MAX_DIST].toFloat();
      serial_laser_.setThreshold(min_dist,max_dist);
      qDebug()<<tr("[%1,%2]laser distance threshold[%3,%4]")
                .arg(__FILE__).arg(__LINE__)
                .arg(min_dist)
                .arg(max_dist);
    }
  }while(0);

  reset();
  serial_laser_.trigger();
  return true;
}

void DistanceEstimater::reset(){
  //reset accumulated
  this->heading_ = 0.0f;
  this->velocity_ = 0.0f;
  this->distance_ = 0.0f;
  this->distance_abs_ = 0.0f;
  this->delta_x_ = 0.0f;
  this->delta_y_ = 0.0f;

  //reset ground distance
  this->laser_distance_ = 0.0f;
  this->laser_update_ts_= 0;
}

void DistanceEstimater::setLensFocal(float lens){
  if(lens<=0.0f)return;
  this->focal_len_ = lens;
  this->focal_length_pix_ =   (lens/1000.0f)/(SIZE_PER_PIXEL/1000000.0f);
}

void DistanceEstimater::distanceDelta(float *x, float *y){
  if(x) *x = delta_x_;
  if(y) *y = delta_y_;
}

bool DistanceEstimater::validateMSE(int ts)
{
  ImuSerialThread::imu_record_t mse_record;
  //step1.search the mse record
  if(!serial_imu_.searchRecord(ts,serial_imu_.mse_list_,&mse_record)){
    qDebug()<<tr("[%1,%2]fail to sync imu record.").arg(__FILE__).arg(__LINE__);
  }
  //step2.check mse threshold
  if(mse_record.axis[0]>=gyro_mse_threshold_x_ ||
     mse_record.axis[1]>=gyro_mse_threshold_y_){
    return true;
  }
  return false;
}

void DistanceEstimater::onLaserUpdated(int timestamp, float dist)
{
  (void)timestamp;
  laser_distance_ = dist;//update the ground distance
  serial_laser_.poweron(true);
}
void DistanceEstimater::onFlowChanged(int ts, float dx, float dy)
{
  if(focal_length_pix_==0 ||
     laser_distance_ <= 0 ||
     (fabs(dy)<this->flow_min_threshold_y_ && fabs(dx)<this->flow_min_threshold_x_)){
    return;
  }

  //step1. validate mse
  if(validateMSE(ts)){
    gyro_mse_pass_ts_ = ts;
  }
  if(abs(ts-gyro_mse_pass_ts_)>gyro_mse_timeout_){
    return;
  }

  //step2.search eular
  float yaw=0.0f, pitch=0.0f;
  do{
    ImuSerialThread::imu_record_t eular_record;
    if(!serial_imu_.searchRecord(ts,serial_imu_.eular_list_,&eular_record)){
      qDebug()<<tr("[%1,%2]fail to find the eular_record at:%3").arg(__FILE__).arg(__LINE__).arg(ts);
      return;
    }
    yaw = eular_record.axis[2];
    pitch = eular_record.axis[1]*M_PI/180.0f;
  }while(0);

  //step3.compute distance
  float dist_y = 0.0f,dist_x=0.0f,dist_all=0.0f;
  //step3.1 compute distance_y
  do{
    float alpha = fabs(pitch);
    float beta=atan(dy/focal_length_pix_);
    float sin_beta=sin(beta);
    float gama = 0.0f;
#if 1
    if(dy>0){
      gama = M_PI/2+alpha-fabs(beta);
    }else{
      gama = M_PI/2-alpha-fabs(beta);
    }
#else
    gama  = M_PI/2+alpha-fabs(beta);
#endif

    if(sin_beta){
      dist_y = laser_distance_*sin_beta/sin(gama);
    }else{
      dist_y = 0.0f;
    }
  }while(0);

  do{
    //step3.2 compute distance_x
    dist_x = dx*laser_distance_/focal_length_pix_;//compute dist_x
  }while(0);


  //step3.3 compute dist all
  dist_all= sqrt(dist_x*dist_x + dist_y*dist_y);
  if(dy<0){
    dist_all = -dist_all;
  }else if(dy==0){
    dist_all = 0;
  }

  //updated distance
  this->delta_x_ = dist_x;
  this->delta_y_ = dist_y;
  this->distance_ += dist_all;
  this->distance_abs_ += fabs(dist_all);
  this->heading_ = yaw;
  this->velocity_ = dist_all*1000.0f/serial_flow_.msecBetweenFrames();
  this->update_timestamp_ = ts;
#if 1
  printf("heading:%.2f, "
         "laser:%.2fm, "
         "delta:%.2f,%.2f, "
         "dist_all:%2fm, "
         "combine:%.2fm\n",
         heading_,
         laser_distance_,
         dx,dy,
         dist_all,
         distance_);
#endif
  emit trajectoryUpdated();
}

void DistanceEstimater::onImuRecordUpdated(int ts)
{
  if(abs(ts-laser_update_ts_)>laser_update_timeout_){
    serial_laser_.trigger();
    laser_update_ts_ = ts;
  }
}
