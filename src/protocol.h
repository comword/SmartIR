/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "transmit.h"

#include <python3.5/Python.h>
#include <map>
#include <string>

using DictsMap = std::map<std::string,const char*>;
class IRProtocol : public transmit
{
public:
  IRProtocol(char* p_buffer);
  virtual ~IRProtocol();
  void do_cycle();
  void set_pipe_buffer(char* p_buffer);
  DictsMap* Proc_PyDict(PyObject* pyValue);
  void action_switch(DictsMap* dicts);
  unsigned int send_toSender(int ClientID,int action,const char *data);
  char *read_fromSender(unsigned int scID);
  char *check_sendir_result(char* datas);
private:
  unsigned int scID_now = 0;
  char* pipe_buffer = nullptr;
  PyThreadState *myThreadState;
  PyObject *pythonMod, *pyProc;
};
//only one class
extern IRProtocol *IRP;
