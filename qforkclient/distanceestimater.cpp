#include "distanceestimater.h"
#include <qdebug.h>
#include <qmath.h>


DistanceEstimater::DistanceEstimater(QObject *parent) : QObject(parent)
{
  connect(&laser_timer_,SIGNAL(timeout()),this,SLOT(onLaserTimeout()));
  connect(&laser_,SIGNAL(distanceChanged(int,float)),this,SLOT(onGroundDistanceChanged(int,float)));
  connect(&flow_,SIGNAL(flowChanged(int,float,float)),this,SLOT(onFlowChanged(int,float,float)));

  this->ground_distance_ = 0.0f;
  this->setLensFocal(LENS_FOCAL_LEN);
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
    if(!laser_.begin(dev)){
      qDebug()<<tr("[%1,%2]fail to opend the laser serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
      goto FAIL;
    }
    qDebug()<<tr("[%1,%2]laser serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
  }while(0);

  do{//flow serial
    QString dev = CONFIG_KEY_SERIAL_FLOW;
    if(configs.contains(CONFIG_KEY_SERIAL_FLOW)){
      dev = configs[CONFIG_KEY_SERIAL_FLOW];
    }
    if(!flow_.begin(dev)){
      qDebug()<<tr("[%1,%2]fail to opend the flow serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
      goto FAIL;
    }
    qDebug()<<tr("[%1,%2]flow serial:%3").arg(__FILE__).arg(__LINE__).arg(dev);
  }while(0);


  do{//set lens focal len
    float lens = LENS_FOCAL_LEN;
    if(configs.contains(CONFIG_KEY_LENS_FOCAL)){
      lens = configs[CONFIG_KEY_LENS_FOCAL].toFloat();
    }
    setLensFocal(lens);
    qDebug()<<tr("[%1,%2]lens_focal_lens:%3mm, focal_length_pix:%4pix").arg(__FILE__).arg(__LINE__).arg(this->lens_focal_len_).arg(this->focal_length_pix_);
  }while(0);

  do{//start laser trigger task
    uint64_t laser_update_timeout = LASER_UPDATE_TIMEOUT;
    if(configs.contains(CONFIG_KEY_LASER_UPDATE_TIMEOUT)){
      laser_update_timeout = configs[CONFIG_KEY_LASER_UPDATE_TIMEOUT].toLong();
    }
    laser_timer_.start(laser_update_timeout);
    qDebug()<<tr("[%1,%2]laser_update_timeout:%3").arg(__FILE__).arg(__LINE__).arg(laser_update_timeout);
  }while(0);

  do{//start dmp trigger task
    int update_timeout = DMPThread::DEFAULT_UPDATE_TIMEOUT;
    int record_size = DMPThread::DEFAULT_RECORD_SIZE;
    int max_timegap = DMPThread::DEFAULT_MAX_TIMEGAP;

    if(configs.contains(CONFIG_KEY_DMP_UPDATE_TIMEOUT)){
      update_timeout = configs[CONFIG_KEY_DMP_UPDATE_TIMEOUT].toInt();
    }
    if(configs.contains(CONFIG_KEY_DMP_RECORD_SIZE)){
      record_size = configs[CONFIG_KEY_DMP_RECORD_SIZE].toInt();
    }
    if(configs.contains(CONFIG_KEY_DMP_MAX_TIMEGAP)){
      max_timegap = configs[CONFIG_KEY_DMP_MAX_TIMEGAP].toInt();
    }
    dmp_source_.startDmpThread(update_timeout,record_size,max_timegap);
    qDebug()<<tr("[%1,%2]dmp update_timeout:%3, record_size:%4, max_timegap:%5")
              .arg(__FILE__).arg(__LINE__)
              .arg(update_timeout)
              .arg(record_size)
              .arg(max_timegap);
  }while(0);

  //step2.reset flow
  clearEstimater();

  return true;
FAIL:
  stop();
  return false;
}

void DistanceEstimater::stop(){
  laser_timer_.stop();
  laser_.close();
  flow_.close();
  dmp_source_.quit();

  clearEstimater();
}

void DistanceEstimater::clearEstimater(){
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

void DistanceEstimater::onGroundDistanceChanged(int timestamp, float dist)
{
  (void)timestamp;
  ground_distance_ = dist;//update the ground distance
  //printf("ground_distance:%.2f\n",ground_distance_);
}

void DistanceEstimater::onFlowChanged(int ts, float dx, float dy)
{
  if(dx==0 && dy==0)return;

  DMPThread::imu_record_t imu;
  float ax=0.0f,ay=0.0f,az=0.0f;
  float gx=0.0f,gy=0.0f,gz=0.0f;
  float yaw=0.0f,pitch=0.0f;

  if(!dmp_source_.syncRecord(ts,imu)){
    qDebug()<<tr("[%1,%2]fail to sync imu record.").arg(__FILE__).arg(__LINE__);
  }else{
    yaw = imu.ypr[0]*180/M_PI;
    pitch = imu.ypr[1];
    gx = dmp_source_.convertGyro(imu.gx);
    gy = dmp_source_.convertGyro(imu.gy);
    gz = dmp_source_.convertGyro(imu.gz);
    ax = dmp_source_.convertAccel(imu.ax);
    ay = dmp_source_.convertAccel(imu.ay);
    az = dmp_source_.convertAccel(imu.az);
  }

  float cosine = cos(pitch);
  if(cosine<=0.0f){
    return;//means the device is verticl
  }

  if(ground_distance_){
    float dist_y = ground_distance_*dy/this->focal_length_pix_;
    dist_y = dist_y / cosine;
    float vel_y = dist_y*1000.0f/flow_.msecBetweenFrames();//velocity in m/s

    int flag = (int)(vel_y*1000);
    if(flag){
      accumulated_dist_y_ += dist_y;
      printf("imu accel:(%.2f,%.2f,%.2f),gyro:(%.2f,%.2f,%.2f),yaw:%.2f,pitch:%.2f ",
             ax,ay,az,
             gx,gy,gz,
             yaw,pitch);
      printf("ground_dist:%.2f,velocity:%.2fm/s, dist:%.2fm\n",ground_distance_,vel_y,accumulated_dist_y_);
    }
  }

}
