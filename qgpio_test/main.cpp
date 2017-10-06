#include <QCoreApplication>
#include <qdebug.h>
#include "neogpio.h"
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  if(argc<2){
    printf("usage:neogpio [gpio] (value)\n"
           "eg:neogpio 67 1\n"
           "   setting 67 to 1\n"
           "eg:neogpio 65\n"
           "   reading 65 value\n");
    return -1;
  }

  int gpio = atol(argv[1]);
  if(argc>=3){
    int value = atol(argv[2]);
    printf("setting gpio%d to %d\n",gpio,value);
    NeoGpio np;
    if(!np.Begin(gpio,NeoGpio::DIRECTION_OUT)){
      printf("[%s,%d]fail to open gpio%d\n",__FILE__,__LINE__,gpio);
      return 0;
    }
    np.Write(value);
  }else{
    printf("getting gpio%d value\n",gpio);
    NeoGpio np;
    if(!np.Begin(gpio,NeoGpio::DIRECTION_IN)){
      printf("[%s,%d]fail to open gpio%d\n",__FILE__,__LINE__,gpio);
      return 0;
    }
    int value = np.Read();
    printf("gpio%d value is %d\n",gpio,value);
  }
  return 0;
  //return a.exec();
}
