#include "distanceestimater.h"
#include <qdebug.h>
#include <qmath.h>

using namespace qri_neuron_lib;
DistanceEstimater::DistanceEstimater(QObject *parent) : QObject(parent)
{
  connect(&laser_timer_,SIGNAL(timeout()),this,SLOT(onLaserTimeout()));
  connect(&laser_,SIGNAL(distanceChanged(int,float)),this,SLOT(onGroundDistanceChanged(int,float)));
  connect(&flow_,SIGNAL(flowChanged(int,float,float)),this,SLOT(onFlowChanged(int,float,float)));

  ptr_gyro_frame_ = NULL;
  this->laser_distance_ = 0.0f;
  this->setLensFocal(LENS_FOCAL_LEN);
}

DistanceEstimater::~DistanceEstimater(){
  if(ptr_gyro_frame_)delete ptr_gyro_frame_;
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

  do{
    gyro_frame_rows_ = GYRO_FRAME_ROWS;
    if(configs.contains(CONFIG_KEY_GYRO_FRAME_SIZE)){
      gyro_frame_rows_ = configs[CONFIG_KEY_GYRO_FRAME_SIZE].toInt();
    }

    gyro_mse_threshold_ = GYRO_MSE_THRESHOLD;
    if(configs.contains(CONFIG_KEY_GYRO_MSE_THRESHOLD)){
      gyro_mse_threshold_ = configs[CONFIG_KEY_GYRO_MSE_THRESHOLD].toFloat();
    }

    gyro_mse_check_ = 1;
    if(configs.contains(CONFIG_KEY_GYRO_MSE_CHECK)){
      gyro_mse_check_ = configs[CONFIG_KEY_GYRO_MSE_CHECK].toInt();
    }

    ptr_gyro_frame_ = new DataFrame(gyro_frame_rows_,GYRO_FRAME_COLS);
    qDebug()<<tr("[%1,%2]gyro queue_size:%3,mse_threshold:%4,mse_check:%5")
              .arg(__FILE__).arg(__LINE__)
              .arg(gyro_frame_rows_)
              .arg(gyro_mse_threshold_)
              .arg(gyro_mse_check_);
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
  this->laser_distance_ = 0.0f;
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
}

void DistanceEstimater::onGroundDistanceChanged(int timestamp, float dist)
{
  (void)timestamp;
  laser_distance_ = dist;//update the ground distance
  //printf("ground_distance:%.2f\n",ground_distance_);
}

void DistanceEstimater::onFlowChanged(int ts, float dx, float dy)
{
  if(dx==0 && dy==0){
    //do not block, as long as it is not zero
    return;
  }
  if(focal_length_pix_==0)return ;


  DMPThread::imu_record_t imu;
  //float accel[3]={0.0f,0.0f,0.0f};
  float gyro[3]={0.0f,0.0f,0.0f};
  float yaw=0.0f,pitch=0.0f;

  //step1. compute DMP accel and gyro
  if(!dmp_source_.syncRecord(ts,imu)){
    qDebug()<<tr("[%1,%2]fail to sync imu record.").arg(__FILE__).arg(__LINE__);
  }else{
    yaw = imu.ypr[0]*180/M_PI;
    pitch = imu.ypr[1];
    gyro[0] = dmp_source_.convertGyro(imu.gx);
    gyro[1] = dmp_source_.convertGyro(imu.gy);
    gyro[2] = dmp_source_.convertGyro(imu.gz);
    //accel[0] = dmp_source_.convertAccel(imu.ax);
    //accel[1] = dmp_source_.convertAccel(imu.ay);
    //accel[2] = dmp_source_.convertAccel(imu.az);
  }

  //step2.check gyro viberiation here
  float gyro_mse=0.0f;
  ptr_gyro_frame_->Push(gyro,GYRO_FRAME_COLS);
  if(ptr_gyro_frame_->Full()){
    gyro_mse = gyro_mse_.Process(ptr_gyro_frame_->Reference(),ptr_gyro_frame_->FrameSize());
    ptr_gyro_frame_->Pop(1);
  }
  if(gyro_mse_check_ && gyro_mse<gyro_mse_threshold_){
    return;
  }

  //step3.compute triangle
  float alpha = fabs(pitch);
  float beta=atan(dy/focal_length_pix_);
  float sin_beta=sin(beta);
  float gama = 0.0f;
  if(dy>0){
    gama = M_PI/2+alpha-beta;
  }else{
    gama = M_PI/2-alpha-beta;
  }
#if 0
  printf("dy=%.2f,focal_lens:=%.2f,gama:%.2f,sin(gama):%.2f,beta:%.2f,sin(beta):%.2f\n",
         dy,this->focal_length_pix_,
         gama,sin(gama),
         beta,sin_beta);
#endif

  //step4. compute real ground distance
  if(laser_distance_){
    float dist_y = 0.0f;
    if(sin_beta){
      dist_y = laser_distance_*sin_beta/sin(gama);
    }else{
      dist_y = laser_distance_;
    }

    float vel_y = dist_y*1000.0f/flow_.msecBetweenFrames();//velocity in m/s

    int flag = (int)(vel_y*1000);
    if(flag){
      accumulated_dist_y_ += dist_y;
      printf("yaw:%.2f,dy:%.2f,laser_dist:%.2f,dist_y:%.2f,velocity:%.2fm/s, ground_dist:%.2fm\n",
             yaw,
             dy,
             laser_distance_,
             dist_y,
             vel_y,
             accumulated_dist_y_);
    }
  }

}
