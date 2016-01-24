/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "IRReader.h"

#include <fstream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>

#define IR_GPIO 17

IRReader::IRReader()
{
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
  IR_buf = (char*) malloc(126*sizeof(char));//125Byte
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
int IRReader::gpio_fd_open(unsigned int gpio)
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
int IRReader::gpio_fd_close(int fd)
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
int IRReader::gpio_get_value(int value_fd)
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
  int gpio_fd = Mclass -> gpio_fd_open(IR_GPIO);
  polls.fd = gpio_fd;
	polls.events = POLLPRI;
  char * buffer = (char*) malloc(126*sizeof(char));//125Byte
  do {
//HIGH ------       -----
// LOW      |_______|   |....Datas
//             9ms  4.5ms
    int i = 0;
    int j = 0;
    int k = 0;
    buffer = (char *)memset(buffer,0,126*sizeof(char));
    poll(&polls,1, -1);//waiting
    if(polls.revents & POLLPRI) { //got it!
      char tmp = 0;
      usleep(2000); //anti-interference
      if(Mclass->gpio_get_value(gpio_fd)==1)//interference
        continue;
      while(Mclass->gpio_get_value(gpio_fd)==1){};
      while(Mclass->gpio_get_value(gpio_fd)==0){};
      while (i < 1001) { //read for 1 secound (1000*100us)
        tmp = Mclass->gpio_get_value(gpio_fd) << (7-j) | tmp;
        j++;
        if(j > 7) {
          j=0;
          memcpy(buffer+k,&tmp,sizeof(char));
          k++;
        }
        i++;
        usleep(100);
      }
      memcpy(Mclass->IR_buf,buffer,126*sizeof(char));
      printf("Found IR signal.k = %d",k);
    }
  } while(1);
  free(buffer);
}
IRReader::~IRReader()
{
  free(IR_buf);
  gpio_unexport(17);
}
