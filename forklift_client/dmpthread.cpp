#include "dmpthread.h"
#include <QTime>
#include <math.h>

#include "edison_i2c.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
DMPThread::DMPThread(QObject * parent): QThread(parent)
{
}

void DMPThread::StartDmpThread(int t,int max_size,int timegap)
{
  update_timeout_ = t;
  record_size_ = max_size;
  record_max_timegap_ = timegap;
  record_list_.clear();
  start();
}

bool DMPThread::SyncRecord(int t, DMPThread::imu_record_t &record)
{
  int min_dist=MSEC_ONE_DAY;
  imu_record_t * ptr_record=NULL;

  record_mutex_.tryLock(RECORD_UPDATE_LOCK_TIMEOUT);

  foreach (imu_record_t iter, record_list_) {
    int d = abs(iter.ts-t);
    if(d<min_dist){
      min_dist = d;
      ptr_record = &iter;
    }
  }
  record_mutex_.unlock();

  if(min_dist<=record_max_timegap_){
    memcpy((uint8_t*)&record,(uint8_t*)ptr_record,sizeof(imu_record_t));
    return true;
  }
  return false;
}

float DMPThread::ConvertAccel(int16_t a){
  return (float)a*accel_range_/MAX_RANGE;
}
float DMPThread::ConvertGyro(int16_t g){
  return (float)g*gyro_range_/MAX_RANGE;
}

void DMPThread::run(){
  MPU6050 mpu;
  uint16_t packetSize;
  uint16_t fifoCount;
  uint8_t mpuIntStatus;
  uint8_t fifoBuffer[256];

  // orientation/motion vars
  Quaternion q;           // [w, x, y, z]         quaternion container
  VectorFloat gravity;    // [x, y, z]            gravity vector

  //set the arduino
  init_arduino();
  Wire.begin();
  printf("Wire initialized.\n");
  //set the mpu
  printf("Initializing I2C devices...\n");

  mpu.initialize();
  printf(mpu.testConnection() ? ("MPU6050 connection successful") : ("MPU6050 connection failed"));
  printf("\nmpu initialized.\n");

  printf("Initializing DMP...");
  uint8_t devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
  if (devStatus == 0) {
      // turn on the DMP, now that it's ready
      printf("Enabling DMP...\n");
      mpu.setDMPEnabled(true);
      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      printf("DMP ready! Waiting for first interrupt...\n");
      packetSize = mpu.dmpGetFIFOPacketSize();
      printf("DMP FIFO Packet Size is %d\n",packetSize);
  } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      printf(F("DMP Initialization failed (code "));
      goto exit;
  }

  accel_range_ = static_cast<float>(pow(2.0,(mpu.getFullScaleAccelRange()+1)));
  gyro_range_ = static_cast<float>(250.0*pow(2.0,mpu.getFullScaleGyroRange()));

  while(true){
      mpuIntStatus = mpu.getIntStatus();
      // get current FIFO count
      fifoCount = mpu.getFIFOCount();

      // check for overflow (this should never happen unless our code is too inefficient)
      if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
          // reset so we can continue cleanly
          printf("[%s,%d]FIFO overflow!\n",__FILE__,__LINE__);
          // otherwise, check for DMP data ready interrupt (this should happen frequently)
      } else if (mpuIntStatus & 0x02) {
          // wait for correct available data length, should be a VERY short wait
          while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

          // read a packet from FIFO
          mpu.getFIFOBytes(fifoBuffer, packetSize);

          // track FIFO count here in case there is > 1 packet available
          // (this lets us immediately read more without waiting for an interrupt)
          fifoCount -= packetSize;

          // display Euler angles in degrees

          imu_record_t imu_record;
          imu_record.ts = QTime::currentTime().msecsSinceStartOfDay();

          mpu.dmpGetQuaternion(&q, fifoBuffer);
          mpu.dmpGetGravity(&gravity, &q);
          mpu.dmpGetYawPitchRoll(imu_record.ypr, &q, &gravity);
          mpu.getMotion6(&(imu_record.ax),&(imu_record.ay),&(imu_record.az),
                         &(imu_record.gx),&(imu_record.gy),&(imu_record.gz));

          record_mutex_.tryLock(RECORD_UPDATE_LOCK_TIMEOUT);
          if(record_list_.size()>=record_size_){
            record_list_.clear();
          }
          record_list_.append(imu_record);
          record_mutex_.unlock();
#if 0
          printf("ypr[0]=%.4f\t ypr[1]=%.4f\t ypr[2]=%.4f\t\n",
                  imu_record.ypr[0] * 180/M_PI,
                  imu_record.ypr[1] * 180/M_PI,
                  imu_record.ypr[2] * 180/M_PI);
#endif
      }
      QThread::msleep(update_timeout_);
      QThread::yieldCurrentThread();
  }

exit:
  Wire.end();
}

