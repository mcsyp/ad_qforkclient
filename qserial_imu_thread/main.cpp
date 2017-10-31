#include <QCoreApplication>
#include "imuserialthread.h"
#include <stdio.h>
#include <QThread>
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  ImuSerialThread serialimu;
  if(serialimu.begin("ttyUSB0")){
    printf("serial imu inited\n");
  }
  return a.exec();
}
