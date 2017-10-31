#include <QCoreApplication>
#include "imuserial.h"
#include <stdio.h>
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  ImuSerial serialimu;
  if(serialimu.begin("ttyUSB0")){
    printf("serial imu inited\n");
  }
  return a.exec();
}
