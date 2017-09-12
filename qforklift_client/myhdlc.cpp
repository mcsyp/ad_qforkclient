#include "myhdlc.h"
#include <QDebug>

/* HDLC Asynchronous framing */
/* The frame boundary octet is 01111110, (7E in hexadecimal notation) */
#define FRAME_BOUNDARY_OCTET 0x7E

/* A "control escape octet", has the bit sequence '01111101', (7D hexadecimal) */
#define CONTROL_ESCAPE_OCTET 0x7D

/* If either of these two octets appears in the transmitted data, an escape octet is sent, */
/* followed by the original data octet with bit 5 inverted */
#define INVERT_OCTET 0x20

/* The frame check sequence (FCS) is a 16-bit CRC-CCITT */
/* AVR Libc CRC function is _crc_ccitt_update() */
/* Corresponding CRC function in Qt (www.qt.io) is qChecksum() */
#define CRC16_CCITT_INIT_VAL 0xFFFF

/* 16bit low and high bytes copier */
#define low(x)    ((x) & 0xFF)
#define high(x)   (((x)>>8) & 0xFF)
static uint16_t _crc_ccitt_update (uint16_t crc, uint8_t data)
{
  data ^= low(crc);
  data ^= data << 4;

  return ((((uint16_t)data << 8) | low (crc)) ^ (uint8_t)(data >> 4)
          ^ ((uint16_t)data << 3));
}
MyHdlc::MyHdlc()
{
  this->max_frame_length_ = MAX_FRAME_SIZE;
  this->frame_position_ = 0;
  this->receive_frame_buffer_ = (uint8_t *)malloc(MAX_FRAME_SIZE+1); // char *ab = (char*)malloc(12);
  this->frame_checksum_ = CRC16_CCITT_INIT_VAL;
  this->escape_character_ = false;
}

/* Function to find valid HDLC frame from incoming data */
void MyHdlc::Push(uint8_t data)
{
  /* FRAME FLAG */
  if(data == FRAME_BOUNDARY_OCTET)
  {
      if(this->escape_character_ == true)
      {
        this->escape_character_ = false;
      }
      /* If a valid frame is detected */
      else if( (this->frame_position_ >= 2) &&
                ( this->frame_checksum_ == ((this->receive_frame_buffer_[this->frame_position_-1] << 8 ) | (this->receive_frame_buffer_[this->frame_position_-2] & 0xff)) ) )  // (msb << 8 ) | (lsb & 0xff)
      {
          /* Call the user defined function and pass frame to it */
        emit hdlcFrameDecoded((char*)receive_frame_buffer_,(this->frame_position_-2));
      }
      this->frame_position_ = 0;
      this->frame_checksum_ = CRC16_CCITT_INIT_VAL;
      return;
  }

  if(this->escape_character_)
  {
      this->escape_character_ = false;
      data ^= INVERT_OCTET;
  }
  else if(data == CONTROL_ESCAPE_OCTET)
  {
      this->escape_character_ = true;
      return;
  }

  receive_frame_buffer_[this->frame_position_] = data;

  if(this->frame_position_-2 >= 0) {
      this->frame_checksum_ = _crc_ccitt_update(this->frame_checksum_, receive_frame_buffer_[this->frame_position_-2]);
  }

  this->frame_position_++;

  if(this->frame_position_ == this->max_frame_length_)
  {
      this->frame_position_ = 0;
      this->frame_checksum_ = CRC16_CCITT_INIT_VAL;
  }
}

/* Wrap given data in HDLC frame and send it out byte at a time*/
void MyHdlc::FrameEncode(const char *framebuffer, uint8_t frame_length)
{
  uint8_t data;
  uint16_t fcs = CRC16_CCITT_INIT_VAL;
  QByteArray tx_frame;

  tx_frame.push_back((char)FRAME_BOUNDARY_OCTET);
  while(frame_length)
  {
    data = *framebuffer++;
    fcs = _crc_ccitt_update(fcs, data);
    if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
    {
        tx_frame.push_back((char)CONTROL_ESCAPE_OCTET);
        data ^= INVERT_OCTET;
    }
    tx_frame.push_back((char)data);
    frame_length--;
  }
  data = low(fcs);
  if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
  {
    tx_frame.push_back((char)CONTROL_ESCAPE_OCTET);
    data ^= (uint8_t)INVERT_OCTET;
  }
  tx_frame.push_back((char)data);
  data = high(fcs);
  if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
  {
    tx_frame.push_back(CONTROL_ESCAPE_OCTET);
    data ^= INVERT_OCTET;
  }
  tx_frame.push_back(data);
  tx_frame.push_back(FRAME_BOUNDARY_OCTET);

  emit hdlcFrameEncoded(tx_frame);
}

