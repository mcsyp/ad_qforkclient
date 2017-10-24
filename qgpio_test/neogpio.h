#ifndef NEOGPIO_H
#define NEOGPIO_H

#include <QObject>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <QFile>
#include <QTextStream>

class NeoGpio : public QObject
{
  Q_OBJECT
public:
  enum DIRECTION{
    DIRECTION_IN=0,
    DIRECTION_OUT=1,
  };
  enum VALUE{
    LOW=0,
    HIGH=1,
  };

  explicit NeoGpio(QObject *parent = 0);
  virtual ~NeoGpio();

  bool Begin(int linux_gpio,int direction=DIRECTION_IN);

  int Read();
  void Write(int value);
  static bool FastWrite(int linux_gpio,int value);
protected:
  void Reset();
protected:
  QFile io_file_;
  QTextStream io_text_;
};

#endif // NEOGPIO_H
