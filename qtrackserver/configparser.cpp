#include "configparser.h"
#include <QTextStream>
#include <QDebug>
ConfigParser::ConfigParser(QObject* parent): QObject(parent){

}

int ConfigParser::Parse(const QString &path, ConfigParser::ConfigMap &config)
{
  if(path.size()==0 ) return 0;
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly)){
    qDebug()<<tr("[%1,%2]Fail to open %3").arg(__FILE__).arg(__LINE__).arg(path);
    return 0;
  }
  QTextStream text(&file);

  while(!text.atEnd()){
    QString str_line = text.readLine();
    //step1. remove space
    str_line.remove(' ');
    if(str_line.size()==0 || str_line.startsWith(COMMENT)){
      continue;
    }

    //step2. split the line into 2 parts
    int index = str_line.indexOf(SEPERATOR);
    QString str_key = str_line.left(index);
    QString str_val = str_line.right(str_line.size()-index-1);
    config.insert(str_key,str_val);
  }
  return config.size();
}
