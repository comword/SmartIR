/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include <vector>
struct zigWBuffer{
  int id;
  int length;
  char* buffer;
};
struct zigRBuffer{
  unsigned int scID;
  char* buffer;
};
class transmit
{
public:
  transmit();
  virtual ~transmit();
  void cfmakeraw(struct termios *t);
	int InitSerial();
  virtual void do_cycle();
  int put_in(char *content,int leng);
  char * Read_rbuffer(unsigned int scID);
private:
  int fd;
  std::vector<zigWBuffer*> wbuffer;
  std::vector<zigRBuffer*> rbuffer;
  void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen);
};
