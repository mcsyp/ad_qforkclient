#ifndef I2CREADER_H
#define I2CREADER_H
#include <QObject>
#include <stdint.h>
class I2CReader : public QObject
{
  Q_OBJECT
public:
  explicit I2CReader(QObject *parent = 0);
  /*purpose: start the i2c reader
   *input:
   *@dev_path: i2c_dev path. eg:/dev/i2c0
   *return:
   * return false if failed
   */
  bool Begin(const char * dev_path);
  bool SetSlave(uint8_t addr);
  uint8_t Slave() const{return slave_addr_;}
  int  Descriptor()const {return i2c_fd_;}

  void WriteByte(uint8_t b);
  void WriteBytes(uint8_t buffer[],int len);

  int ReadByte();
  int ReadBytes(uint8_t out[],int out_size);
protected:
  int i2c_fd_;
  uint8_t slave_addr_;
};
#endif // I2CREADER_H
