#include <QCoreApplication>
extern "C" {
    #include <stdio.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <math.h>
}
#include "edison_i2c.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
using namespace std;
int main(int argc, char *argv[])
{
    MPU6050 mpu;
    uint16_t packetSize;
    uint16_t fifoCount;
    uint8_t mpuIntStatus;
    uint8_t fifoBuffer[256];

    // orientation/motion vars
    Quaternion q;           // [w, x, y, z]         quaternion container
    VectorFloat gravity;    // [x, y, z]            gravity vector
    float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
    int counter=0;

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
        //goto exit;

    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        printf(F("DMP Initialization failed (code "));
        goto exit;
    }

    while(counter<100){
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
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            printf("ypr[0]=%.4f\t ypr[1]=%.4f\t ypr[2]=%.4f\t\n",
                ypr[0] * 180/M_PI,
                ypr[1] * 180/M_PI,
                ypr[2] * 180/M_PI);
        }
        usleep(5000);
    }

exit:
    Wire.end();
    return 0;
}
