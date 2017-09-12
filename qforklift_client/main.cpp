#include <QCoreApplication>
#include <qdebug.h>
#include <QDir>
#include <QStringList>

#include <signal.h>

#include "forkliftclient.h"
#include "configparser.h"

ForkliftClient g_socket_client_;
void signal_exit(int sig){
  (void)sig;
  //g_socket_client_.slave_ble_.ResetBle();
  qDebug()<<"Application quits.Reseting the ble...";
  QThread::msleep(1000);
  exit(0);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    signal(SIGINT,signal_exit);
    //step1.check input
    if(argc<2){
      qDebug()<<"usgae: qforkclient [config_file]";
      qDebug()<<"usage: qforkclient ../config.txt";
      exit(0);
    }

    //step2. load the config file
    qDebug()<<"Parsing the config file...";
    ConfigParser::ConfigMap configs;
    ConfigParser parser;
    if(!parser.Parse(argv[1],configs)){
      qDebug()<<"Fail to load the config file:"<<argv[1]<<endl;
      exit(0);
    }

    //step2. dump all config keys and values
    QStringList keys=configs.keys();
    for(int i=0;i<keys.size();++i){
        qDebug()<<"["<<i<<"]"<<keys[i]<<"="<<configs[keys[i]];
    }

    //step3. find the work_path
    QString config_path(argv[1]);
    QDir work_path(config_path);
    QString work_dir = work_path.absolutePath();
    work_dir.remove(work_dir.split('/').last());
    qDebug()<<"Current config root directory:"<<work_dir;

    //step4. load devices
    if(!g_socket_client_.Begin(configs)){
      qDebug()<<"Fail to begin the esitmater";
      return 0;
    }
    qDebug()<<"Running the estimater...";
    return a.exec();
}
