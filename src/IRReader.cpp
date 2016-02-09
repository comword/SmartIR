/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "IRReader.h"
#include "pylinker.h"

#include <fstream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <iostream>
#include <sched.h>

#define IR_GPIO 17

IRReader::IRReader()
{
  IR_buf = (char*) malloc(301*sizeof(char));
}
void IRReader::start_learn_IR(int IRID)
{
  this->now_IRID = IRID;
  if(gpio_export(IR_GPIO) < 0)
  throw std::runtime_error(std::string("IRReader::assert gpio_export(IR_GPIO) < 0\n"));
  if(gpio_set_dir(IR_GPIO,0) < 0)
  throw std::runtime_error(std::string("IRReader::assert gpio_set_dir(IR_GPIO,0) < 0\n"));
  if(gpio_set_edge(IR_GPIO,"falling") < 0)
  throw std::runtime_error(std::string("IRReader::assert gpio_set_edge(IR_GPIO,'falling') < 0\n"));
  pthread_attr_t attr;
  pthread_t tid;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  int res = pthread_create(&tid, &attr, IRReader::wait_for_IR, this);
  if (res)
  throw std::runtime_error(std::string("IRReader::pthread_create() Failed!\n"));
}
int IRReader::piHiPri (int pri)
{
  struct sched_param sched ;
  memset (&sched, 0, sizeof(sched)) ;
  if (pri > sched_get_priority_max (SCHED_RR))
  pri = sched_get_priority_max (SCHED_RR) ;
  sched.sched_priority = pri ;
  return sched_setscheduler (0, SCHED_RR, &sched) ;
}
int IRReader::gpio_set_edge(unsigned int gpio,const char *edge)
{
  int fd;
  char buf[64];
  snprintf(buf, sizeof(buf),"/sys/class/gpio" "/gpio%d/edge", gpio);
  fd = open(buf, O_WRONLY);
  if (fd < 0) {
    perror("gpio/set-edge");
    return fd;
  }
  write(fd, edge, strlen(edge) + 1);
  close(fd);
  return 0;
}
inline int IRReader::gpio_fd_open(unsigned int gpio)
{
  int fd;
  char buf[64];
  snprintf(buf, sizeof(buf), "/sys/class/gpio" "/gpio%d/value", gpio);
  fd = open(buf, O_RDONLY | O_NONBLOCK );
  if (fd < 0) {
    perror("gpio/fd_open");
  }
  return fd;
}
inline int IRReader::gpio_fd_close(int fd)
{
  return close(fd);
}
int IRReader::gpio_export(unsigned int gpio)
{
  int fd, len;
  char buf[64];
  fd = open("/sys/class/gpio" "/export", O_WRONLY);
  if (fd < 0) {
    perror("gpio/export");
    return fd;
  }
  len = snprintf(buf, sizeof(buf), "%d", gpio);
  write(fd, buf, len);
  close(fd);
  return 0;
}
int IRReader::gpio_unexport(unsigned int gpio)
{
  int fd, len;
  char buf[64];
  fd = open("/sys/class/gpio" "/unexport", O_WRONLY);
  if (fd < 0) {
    perror("gpio/export");
    return fd;
  }
  len = snprintf(buf, sizeof(buf), "%d", gpio);
  write(fd, buf, len);
  close(fd);
  return 0;
}
int IRReader::gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
  int fd;
  char buf[64];
  snprintf(buf, sizeof(buf),"/sys/class/gpio" "/gpio%d/direction", gpio);
  fd = open(buf, O_WRONLY);
  if (fd < 0) {
    perror("gpio/direction");
    return fd;
  }
  if (out_flag)
  write(fd, "out", 4);
  else
  write(fd, "in", 3);
  close(fd);
  return 0;
}
int IRReader::gpio_set_value(int value_fd, unsigned int value)
{
  if (value)
  write(value_fd, "1", 2);
  else
  write(value_fd, "0", 2);
  return 0;
}
inline int IRReader::gpio_get_value(int value_fd)
{
  char ch;
  read(value_fd, &ch, 1);
  if (ch != '0') {
    return 1;
  } else {
    return 0;
  }
  return 0;
}
void *IRReader::wait_for_IR (void * ptr)
{
  struct pollfd polls;
  IRReader *Mclass = (IRReader *)ptr;
  char * buffer = (char*) malloc(126*sizeof(char));
  uint8_t c;
  int gpio_fd = Mclass -> gpio_fd_open(IR_GPIO);
  polls.fd = gpio_fd;
  polls.events = POLLPRI;
  Mclass->piHiPri(20);
  //HIGH ------       -----
  // LOW      |_______|   |....Datas
  //             9ms  4.5ms
  start:
  int i = 0;
  int j = 0;
  int k = 0;
  buffer = (char *)memset(buffer,0,301*sizeof(char));
  poll(&polls,1,-1);//waiting
  if(polls.revents & POLLPRI) { //got it!
    lseek(gpio_fd,0,SEEK_SET);
    char tmp = 0;
    usleep(1500); //anti-interference
    read(gpio_fd,&c,1);
    lseek(gpio_fd,0,SEEK_SET);
    if(c == '1') {//interference
      goto start;
    }
    do {
      read(gpio_fd,&c,1);
      lseek(gpio_fd,0,SEEK_SET);
    } while (c == '0');
    do {
      read(gpio_fd,&c,1);
      lseek(gpio_fd,0,SEEK_SET);
    } while (c == '1');
    while (i < 2401) { //read for .072 secound (2400*30us)
      read(gpio_fd,&c,1);
      lseek(gpio_fd,0,SEEK_SET);
      tmp = (c == '0') ? 0 : 1 << j | tmp;
      j++;
      if(j > 7) {
        j=0;
        memcpy(buffer+k,&tmp,sizeof(char));
        k++;
      }
      i++;
      usleep(30);
    }
    memcpy(Mclass->IR_buf,buffer,301*sizeof(char));
    printf("Found IR signal.\n");
    char todisplay[602];
    char * p_display = todisplay;
    memset(p_display,0,602*sizeof(char));
    Mclass->ByteToHexStr((const unsigned char*)buffer,p_display,301);
    std::cout<<todisplay<<std::endl;
  }
  free(buffer);
  Mclass->finish_learn_callback();
  return 0;
}
IRReader::~IRReader()
{
  free(IR_buf);
  gpio_unexport(17);
}
void IRReader::ByteToHexStr(const unsigned char* source, char* dest, int sourceLen)
{
  short i;
  unsigned char highByte, lowByte;
  for (i = 0; i < sourceLen; i++){
    highByte = source[i] >> 4;
    lowByte = source[i] & 0x0f;
    highByte += 0x30;
    if (highByte > 0x39)
    dest[i * 2] = highByte + 0x07;
    else
    dest[i * 2] = highByte;
    lowByte += 0x30;
    if (lowByte > 0x39)
    dest[i * 2 + 1] = lowByte + 0x07;
    else
    dest[i * 2 + 1] = lowByte;
  }
  return ;
}
void IRReader::finish_learn_callback()
{
  if(now_IRID != -1){
    char str[10];
    sprintf(str,"%d", now_IRID);
    web -> write_database("/IR_detail.db",std::string(str),std::string(IR_buf));
  }
}
