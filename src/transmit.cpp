/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "transmit.h"
#include "IRReader.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <stdexcept>

transmit::transmit()
{
	//fd = this->InitSerial();
}
transmit::~transmit()
{
  close(fd);
}
inline void transmit::cfmakeraw(struct termios *t)
{
    t->c_iflag &= ~(IMAXBEL|IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    t->c_oflag &= ~OPOST;
    t->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    t->c_cflag &= ~(CSIZE|PARENB);
    t->c_cflag |= CS8;
}
int transmit::InitSerial()
{
	int u = -1;
	u = open("/dev/ttyAMA0",O_RDWR | O_NOCTTY | O_NDELAY);
	if (u == -1){
		throw std::runtime_error("transmit.cpp:Error open UART port.\n");
		return -1;
	}
	struct termios options;
	cfmakeraw(&options); //row mode
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	options.c_cflag |=  (CLOCAL | CREAD);
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_iflag |= IGNPAR;
	options.c_cflag &= ~CSTOPB; //one stop bit
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(u, TCIFLUSH);
	tcsetattr(u, TCSANOW, &options);
	struct serial_struct ss;
	ioctl(u, TIOCGSERIAL, &ss);
	float BAUDRATE = 115200;
	ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
	ss.custom_divisor = ss.baud_base / BAUDRATE;
//	printf("ss.baud_base:%d\nss.custom_divisor:%d\n",ss.baud_base,ss.custom_divisor);
	ioctl(u, TIOCSSERIAL, &ss);
	return u;
}
void transmit::do_cycle()
{
  for (std::vector<zigBuffer*>::iterator it=wbuffer.begin();it != wbuffer.end();){
		zigBuffer *tmp = *it;
		//char todisplay[50];
		//char * p_display=todisplay;
		//memset(p_display,0,50*sizeof(char));
		//ByteToHexStr((const unsigned char*)tmp->buffer,p_display,tmp->length);
		//std::cout<<p_display<<std::endl;
		for (int j=0; j < tmp->length; j++){
			//write(*fd,tmp->buffer+j,1);
			//tcflush(*fd,TCOFLUSH);
		}
		delete tmp->buffer;
		delete tmp;
		it = wbuffer.erase(it);
		usleep(1000);
	}
}
