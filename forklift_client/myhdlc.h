#ifndef MYHDLC_H
#define MYHDLC_H

#include <QObject>
#include <QtCore/QtGlobal>
#include <QString>
#include <QMutex>
#include <QQueue>

namespace hdlc_qt {
  /* HDLC Asynchronous framing */
  /* The frame boundary octet is 01111110, (7E in hexadecimal notation) */
  #define FRAME_BOUNDARY_OCTET 0x7E

  /* A "control escape octet", has the bit sequence '01111101', (7D hexadecimal) */
  #define CONTROL_ESCAPE_OCTET 0x7D

  /* If either of these two octets appears in the transmitted data, an escape octet is sent, */
  /* followed by the original data octet with bit 5 inverted */
  #define INVERT_OCTET 0x20

  /* 16bit low and high bytes copier */
  #define low(x)    ((x) & 0xFF)
  #define high(x)   (((x)>>8) & 0xFF)


  class MyHdlc : public QObject
  {
      Q_OBJECT
  public:
    MyHdlc(){}
    MyHdlc(const MyHdlc &); // hide copy constructor
    MyHdlc& operator=(const MyHdlc &);// hide assign op
                                        // we leave just the declarations, so the compiler will warn us
                                        // if we try to use those two functions by accident

  signals:
    /* Signals can never have return types (i.e. use void) */
    void hdlcFrameEncoded(QByteArray data);
    void hdlcFrameDecoded(QByteArray receive_frame_buffer, quint16 frame_size);

  public slots:
    void PushToDecode(QByteArray dataArray);
    void EncodeFrame(QByteArray buffer, quint16 bytes_to_send);
  private:
    bool hdlcFrameCRC_check(int frame_index);

  private:
    QByteArray receive_frame_buffer;
    quint16 frame_position;
    quint16 frame_checksum;
    bool escape_character;

  };
}

#endif // MYHDLC_H
