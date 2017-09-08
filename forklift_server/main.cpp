#include <QCoreApplication>
#include "simserver.h"
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  SimServer server;
  server.StartService(SimServer::SERVER_PORT);

  return a.exec();
}
