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

#define IR_GPIO 17

IRReader::IRReader(const char* wiringPi_path)
{
  std::string exp;
	std::ifstream fin(wiringPi_path);
  if (!fin){
		exp="IRReader.cpp:wiringPi INIT Failed: No such file or directory.";
		throw std::runtime_error(exp);
		return;
	}
  wiringPi_handle=dlopen(wiringPi_path, RTLD_LAZY);
  if (wiringPi_handle == nullptr){
		exp=std::string("IRReader.cpp:dlopen failed:")+dlerror();
		throw std::runtime_error(exp);
		return;
	}
  dlerror();
  wiringPiSetupSys = (int(*)())dlsym(wiringPi_handle, "wiringPiSetupSys");
  wiringPiISR = (int(*)(int,int,void (*)(void)))dlsym(wiringPi_handle, "wiringPiISR");
  piThreadCreate = (int (*) (void *(*)(void *)))dlsym(wiringPi_handle, "piThreadCreate");
  piHiPri= (int (*) (int))dlsym(wiringPi_handle, "piHiPri");
  waitForInterrupt = (int(*)(int, int))dlsym(wiringPi_handle, "waitForInterrupt");
  digitalRead = (int (*) (int))dlsym(wiringPi_handle, "digitalRead");
  if(wiringPiSetupSys () == -1){
		exp = std::string("IRReader.cpp:assert wiringPiSetup () == -1:") + strerror(errno);
		dlclose(wiringPi_handle);
		throw std::runtime_error(exp);
		return;
	}
  pthread_attr_t attr;
  pthread_t tid;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  int res = pthread_create(&tid, &attr, IRReader::wait_for_IR, this);
  if (res)
    throw std::runtime_error(std::string("pylinker::pthread_create() Failed!\n"));
  IR_buf = (char*) malloc(126*sizeof(char));//125Byte
}
int IRReader::gpio_set_edge(unsigned int gpio, char *edge)
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
void *IRReader::wait_for_IR (void * ptr)
{
  IRReader *Mclass = (IRReader *)ptr;
  Mclass->piHiPri(10);
  char * buffer = (char*) malloc(126*sizeof(char));//125Byte
  do {
//HIGH ------       -----
// LOW      |_______|   |....Datas
//             9ms  4.5ms
    int i = 0;
    int j = 0;
    int k = 0;
    buffer = (char *)memset(buffer,0,126*sizeof(char));
    if(Mclass->waitForInterrupt(IR_GPIO,-1) > 0) {
      char tmp = 0;
      usleep(2000); //anti-interference
      if(Mclass->digitalRead(IR_GPIO)==HIGH)//interference
        continue;
      while(Mclass->digitalRead(IR_GPIO)==HIGH){};
      while(Mclass->digitalRead(IR_GPIO)==LOW){};
      while (i < 1001) { //read for 1 secound (1000*100us)
        tmp = Mclass->digitalRead(IR_GPIO) << (7-j) | tmp;
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
}
