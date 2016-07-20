/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

class IRReader
{
public:
  IRReader(int l_c);
  virtual ~IRReader();
  int piHiPri (int pri);
  int gpio_set_edge(unsigned int gpio,const char *edge);
  int gpio_fd_open(unsigned int gpio);
  int gpio_fd_close(int fd);
  int gpio_export(unsigned int gpio);
  int gpio_unexport(unsigned int gpio);
  int gpio_set_dir(unsigned int gpio, unsigned int out_flag);
  int gpio_set_value(int value_fd, unsigned int value);
  int gpio_get_value(int value_fd);
  static void *wait_for_IR (void * ptr);
  static void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen);
  void start_learn_IR(int IRID);
  void finish_learn_callback();
  void proc_learn_res();
private:
  int fd;
  int learn_count = 0;
  char** IR_buf;
  int now_IRID = -1;
};
extern IRReader *IR;
