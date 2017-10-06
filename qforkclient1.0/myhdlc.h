#ifndef MYHDLC_H
#define MYHDLC_H
#include <QObject>
#include <QByteArray>
#include <stdint.h>
#include <stdbool.h>

class MyHdlc: public QObject
{
  Q_OBJECT
public:
  static constexpr int MAX_FRAME_SIZE=512;
  MyHdlc();
  void Push(uint8_t data);
  void FrameEncode(const char *framebuffer, uint8_t frame_length);
signals:
  /* Signals can never have return types (i.e. use void) */
  void hdlcFrameEncoded(QByteArray data);
  void hdlcFrameDecoded(char* data,int len);

private:
  bool escape_character_;
  uint8_t * receive_frame_buffer_;
  uint8_t frame_position_;

  // 16bit CRC sum for _crc_ccitt_update
  uint16_t frame_checksum_;
  uint16_t max_frame_length_;
};

#endif // MYHDLC_H
