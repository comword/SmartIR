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
	fd = this->InitSerial();
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
	//send
  for (std::vector<zigWBuffer*>::iterator it=wbuffer.begin();it != wbuffer.end();){
		zigWBuffer *tmp = *it;
		for (int j=0; j < tmp->length; j++){
			write(fd,tmp->buffer+j,1);
			tcflush(fd,TCOFLUSH);
		}
		delete tmp->buffer;
		delete tmp;
		it = wbuffer.erase(it);
		usleep(1000);
	}
	//read the return
	zigRBuffer *tmp = new zigRBuffer;
	tmp -> buffer = (char*)malloc(1024*sizeof(char));
	int res = read(fd, tmp -> buffer, 1024);
	tmp -> buffer[res]=0;
	unsigned int tmpID;
	memcpy(&tmpID,tmp -> buffer,2);
	tmp -> scID = tmpID;
	rbuffer.push_back(tmp);
}
int transmit::put_in(char *content,int leng)
{
	zigWBuffer *tmp = new zigWBuffer;
	// FF FF Content Checksum
	tmp->buffer = new char[leng+3];
	memset(tmp->buffer,0xff,2);
	memcpy(tmp->buffer+2,content,leng);
	finish_checksum(tmp->buffer,leng);
	tmp -> length = leng;
	wbuffer.push_back(tmp);
	return 0;
}
void transmit::ByteToHexStr(const unsigned char* source, char* dest, int sourceLen)
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
void transmit::Read_rbuffer(char *buffer,unsigned int scID)
{
	for (std::vector<zigRBuffer*>::iterator it=rbuffer.begin();it != rbuffer.end();){
		zigRBuffer *tmp = *it;
		if(tmp->scID == scID)
		{
			memcpy(buffer,tmp->buffer,tmp->length);
			delete tmp->buffer;
			delete tmp;
			it = rbuffer.erase(it);
		}
	}
}
template <typename T>
void transmit::clean_buffer(std::vector<T> &buffer)
{
	for( const auto &list : buffer ) {
		delete list.buffer;
		delete list;
	}
	buffer.clear();
}

void transmit::finish_checksum(char *buffer,int buf_size)
{
  int sum = 0;
  for (int i = 2;i < buf_size - 1;i++)
    sum += *(buffer+i);
  sum = sum & 0xff;
  sum = ~sum;
  *(buffer+buf_size-1) = sum;
}

void transmit::HexToOut(int length, char* buffer)
{
	char *p_display = (char *)malloc(sizeof(char) * length);
	memset(p_display, 0 ,sizeof(char) * length);
	ByteToHexStr((const unsigned char*)buffer,p_display,length);
	printf("Hex out: %s \n",p_display);
}

bool transmit::verify_uart_mesg(char *buffer,int length)
{
	if(buffer[0]==(char)0xff && buffer[1]==(char)0xff){
		int sum = 0;
		for (int i = 2;i < length - 1;i++)
	    sum += *(buffer + i);
	  sum = sum & 0xff;
	  sum = ~sum;
		if(buffer[length - 1] == sum)
			return true;
	}
	return false;
}
