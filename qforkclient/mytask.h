#ifndef MYTASK_H
#define MYTASK_H

#include <QObject>

class MyTask : public QObject
{
  Q_OBJECT
public:
  explicit MyTask(QObject *parent = 0);
  void Begin(int interval_ms);
  void Trigger(int current_ms);

signals:
  void triggered(int ms);

protected:
  int interval_;
  int last_trigger_;
};

#endif // MYTASK_H
