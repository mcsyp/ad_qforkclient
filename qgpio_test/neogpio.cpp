#include "neogpio.h"
#include <qdebug.h>
#include <QThread>
#include <QIODevice>

#define CMD_GPIO_EXPORT "sudo echo %d >> /sys/class/gpio/export"
#define CMD_GPIO_DIRECTION "sudo echo %s >> /sys/class/gpio/gpio%d/direction"
#define CMD_GPIO_VALUE  "sudo echo %d >> /sys/class/gpio/gpio%d/value"
#define GPIO_VALUE_FILE "/sys/class/gpio/gpio%d/value"
NeoGpio::NeoGpio(QObject *parent) : QObject(parent){}
NeoGpio::~NeoGpio(){
  Reset();
}
void NeoGpio::Reset(){
  if(io_file_.isOpen()){
    io_file_.close();
  }
}
bool NeoGpio::Begin(int linux_gpio, int direction){
  if(linux_gpio<0)return false;
  switch(direction){
    case DIRECTION_IN:
    case DIRECTION_OUT:
      break;
    default:
      return false;
  }

  //reset file
  Reset();

  //step1.export gpio file
  do{
    char cmd_buffer[80];
    sprintf(cmd_buffer,CMD_GPIO_EXPORT,linux_gpio);
    system(cmd_buffer);
    qDebug()<<tr("[%1,%2]executing: %3").arg(__FILE__).arg(__LINE__).arg(cmd_buffer);
  }while(0);

  //step2.setting direction
  do{
    char cmd_buffer[80];
    switch(direction){
      case DIRECTION_IN:
        sprintf(cmd_buffer,CMD_GPIO_DIRECTION,"in",linux_gpio);
        break;
      case DIRECTION_OUT:
        sprintf(cmd_buffer,CMD_GPIO_DIRECTION,"out",linux_gpio);
        break;
      default:
        return false;
    }
    system(cmd_buffer);
    qDebug()<<tr("[%1,%2]executing: %3").arg(__FILE__).arg(__LINE__).arg(cmd_buffer);
  }while(0);

  //step3.set defuault value
  if(direction==DIRECTION_OUT){
    char cmd_buffer[80];
    sprintf(cmd_buffer,CMD_GPIO_VALUE,0,linux_gpio);
    system(cmd_buffer);
    qDebug()<<tr("[%1,%2]executing: %3").arg(__FILE__).arg(__LINE__).arg(cmd_buffer);
  };
  //sleep 5 msec
  QThread::msleep(5);

  //step4.open value file
  char io_str[64];
  sprintf(io_str,GPIO_VALUE_FILE,linux_gpio);
  io_file_.setFileName(QString(io_str));
  if(!io_file_.open(QIODevice::ReadWrite)){
    qDebug()<<tr("[%1,%2]Fail to open io_file:%3").arg(__FILE__).arg(__LINE__).arg(io_str);
    return false;
  }
  io_text_.setDevice(&io_file_);

  return true;
}

int NeoGpio::Read(){
  if(io_file_.isOpen()){
    io_text_.seek(0);
    int val = io_text_.readAll().toInt();
    return val;
  }
  return 0;
}

void NeoGpio::Write(int value){
  if(io_file_.isOpen()){
    io_text_.seek(0);
    if(value)
      io_text_<<1<<endl;
    else
      io_text_<<0<<endl;
  }
}
