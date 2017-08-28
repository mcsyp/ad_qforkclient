#include <QCoreApplication>
#include <qdebug.h>
#include <QThread>

#include "serialpix4flow.h"
#include "seriallaser.h"
#include "distanceestimater.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  DistanceEstimater estimator;
  if(!estimator.begin()){
    qDebug()<<"Fail to create the estimator";
    exit(1);
  }

  return a.exec();
}
