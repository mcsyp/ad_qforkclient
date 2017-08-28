#ifndef SERIALPIX4FLOW_H
#define SERIALPIX4FLOW_H

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTime>

class SerialPix4Flow : public QSerialPort
{
  Q_OBJECT
public:
#define FLOW_BAUDRATE QSerialPort::Baud115200
  static constexpr uint8_t DEFAULT_THRESHOLD_QUALITY=100;

  static constexpr int16_t TEMPERATURE_GAP=300;
  static constexpr float FLOW_XY_SCALE=10.0f;
  static constexpr float FLOW_TEMP_SCALE=100.0f;

  explicit SerialPix4Flow(QObject *parent = 0);

  /*purpose: begin this serial port to connect to PIX4FLOW
   *input:
   *@portname:the port's name, eg:ttyS1
   *return:
   * true if the port is open successfully
   */
  bool begin(const QString portname);

  uint64_t msecBetweenFrames() const{return time_between_frames_;}
signals:
  void flowChanged(uint64_t t,float dx,float dy);
  void temperatureChanged(float temp);
public slots:
  void onReadyRead();

protected:
  uint64_t time_between_frames_;
  uint64_t last_flow_update_time_;
};

#endif // SERIALPIX4FLOW_H
