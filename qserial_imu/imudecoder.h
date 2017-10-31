#ifndef SERIALMPUDECODER_H
#define SERIALMPUDECODER_H

#include <QObject>

class ImuDecoder : public QObject
{
  Q_OBJECT
public:
  static const uint8_t START_CODE=0x55;
  static const int MAX_PACK_SIZE=11;
  static constexpr float MAX_RANGE = 32768.0f;
  static constexpr int IMU_DECODED_AXIS=4;
  enum PACK{
    PACK_ACCEL = 0x51,
    PACK_GYRO  = 0x52,
    PACK_EULAR = 0x53,
  };

  explicit ImuDecoder(QObject *parent = 0);

  void reset();

  /* purpose: push one byte to decode
   * input:
   *  @b, one byte to decode
   */
  void push(uint8_t b);

  /* purpose: compute the crc for the bytes buffer
   * input:
   *  @bytes, the bytes buffer
   *  @len, the length of the bytes buffer
   * return:
   *  crc value in one byte
   */
  static uint8_t computeCRC(uint8_t * bytes,int len);

  /* purpose: read the just upated imu data
   * output:
   *  @out_array, the output array for saving the decoded info
   *  @out_size, the size of the output array
   * return:
   *  the length of the output array
   */
  int readData(float out_array[],int out_size);

  //return the rx imudata id
  int readID() const;

protected:
  /* purpose: decode the rx bytes queue with MAX_PACK_SIZE
   * input:
   *  @butter, the rx bytes buffer
   *  @len, the rx bytes buffer length, must be MAX_PACK_SIZE
   * output:
   *  @out_array, the output array for saving the decoded info
   *  @out_size, the size of the output array
   * return:
   *  the length of the output array
   */
  int decode(uint8_t * buffer, int len, float out_array[],int out_size);
signals:
  void readyRead();

protected:
  uint8_t rx_queue_[MAX_PACK_SIZE];
  int rx_len_;
  bool rx_started_;

private:
  int pack_id_;
  float imu_data_[IMU_DECODED_AXIS];
};

#endif // SERIALMPUDECODER_H
