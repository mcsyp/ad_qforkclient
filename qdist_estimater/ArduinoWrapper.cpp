extern "C" {
#include <sys/time.h>
#include <unistd.h>
}
#include "ArduinoWrapper.h"

using namespace std;

EdisonI2C Wire;

static long g_startMillis=0;

void init_arduino()
{
	struct timeval tval;
	gettimeofday(&tval,NULL);
	g_startMillis = tval.tv_sec*1000+tval.tv_usec/1000;
	return;
}
void delay(unsigned long ms)
{
	usleep(ms*1000);
}
#if 1
uint32_t millis()
{
	struct timeval tval;
	gettimeofday(&tval,NULL);
	long t = tval.tv_sec*1000+tval.tv_usec/1000;
	return (uint32_t)(t-g_startMillis);
}
#endif

uint8_t min(uint8_t a,uint8_t b)
{
	if(a>b)return b;
	return a;
}
