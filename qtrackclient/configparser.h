#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <QObject>
#include <QFile>
#include <QMap>
class ConfigParser : public QObject
{
  Q_OBJECT
public:
  typedef QMap<QString,QString> ConfigMap;
  static constexpr char SEPERATOR='=';
  static constexpr char COMMENT=';';

  explicit ConfigParser(QObject* parent=NULL);

  /*purpose: parse the input path into config map
   *input:
   * @path, the file path
   *output:
   * @config, the parsed config map
   *return:
   * how many keys analyzed in the file
   * return 0 if no available keys founded
   */
  int Parse(const QString & path, ConfigMap& config);
};

#endif // CONFIGPARSER_H
