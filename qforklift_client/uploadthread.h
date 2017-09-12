#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
class UploadThread : public QThread
{
  Q_OBJECT
public:
  explicit UploadThread(QObject *parent = 0);
  void UploadFile(QString path,QTcpSocket* socket);
  virtual void run();
protected:
  QTcpSocket * ptr_socket_;
  QString upload_path_;
};

#endif // UPLOADTHREAD_H
