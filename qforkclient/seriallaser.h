#ifndef SERIALLASER_H
#define SERIALLASER_H

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTime>

class SerialLaser : public QSerialPort
{
  Q_OBJECT
public:
#define LASER_BAUDRATE QSerialPort::Baud19200
#define LASER_ON "O"
#define LASER_DETECT "D"
  static constexpr float DEFAULT_MAX_DIST_THRESHOLD = 1.0f;//1m if next value gap bigger than this. invalid
  static constexpr float DEFAULT_MIN_DIST_THRESHOLD = 0.0001f;
  explicit SerialLaser(QObject *parent = 0);

  /*purpose: begin this serial port to connect to PIX4FLOW
   *input:
   *@portname:the port's name, eg:ttyS1
   *return:
   * true if the port is open successfully
   */
  bool begin(const QString portname);
  void poweron();
  void trigger();
  void setThreshold(float min,float max);
protected:
  bool checkValid(float current);
  float parseResponseMessage(char* str);
signals:
  void distanceChanged(int t,float distance);

public slots:
  void onReadyRead();
protected:
  float min_gap_;
  float max_gap_;

};

#endif // SERIALLASER_H
