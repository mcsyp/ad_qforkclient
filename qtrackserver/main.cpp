#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>

#include "configparser.h"
#include "trajectoryserver.h"
int main(int argc, char *argv[])
{
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);

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

  TrajectoryServer track_server_;

  QQmlApplicationEngine engine;
  QQmlContext * ctx = engine.rootContext();

  ctx->setContextProperty("track_server_",&track_server_);
  engine.load(QUrl(QLatin1String("qrc:/main.qml")));

  track_server_.StartTrackService(configs);
  return app.exec();
}
