#include "mytask.h"
#include <QTime>
MyTask::MyTask(QObject *parent) : QObject(parent)
{
  interval_ = 0;
  last_trigger_ = 0;
}

void MyTask::Begin(int interval_ms)
{
  if(interval_ms>=0){
    interval_ = interval_ms;
    last_trigger_ = QTime::currentTime().msecsSinceStartOfDay();
  }
}

void MyTask::Trigger(int current_ms)
{
  if(current_ms-last_trigger_>=interval_){
    last_trigger_ = current_ms;
    emit triggered(current_ms);
  }
}
