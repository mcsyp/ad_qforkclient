#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int args,char *argv[])
{ 

  if(args<4){
    printf("usage: gpio_read [gpio]\n"
           "\teg:gpio_read 65\n");
    goto exit;
  }
  int gpio = atoi(argv[1]);//buadrate

  //set ble host mode
  char str_cmd[64] = "sudo ehco 65 >> /sys/class/gpio/export";
  sprintf(str_cmd,"sudo ehco %d >> /sys/class/gpio/export",gpio);
  system(str_cmd);
  
  
  const char* str_reset = "AT+RESET\r\n";
  send_data(psttty->fd, str_reset,strlen(str_reset));
  printf("[%s,%d]ble reset\n",__FILE__,__LINE__);
  sleep(5);
exit:
  if(psttty)free(psttty);
  return 0;
}
