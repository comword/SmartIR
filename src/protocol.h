/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "transmit.h"

#include <python2.7/Python.h>
#include <map>
#include <string>

using DictsMap = std::map<std::string,std::string>;
class IRProtocol : public transmit
{
public:
  IRProtocol(char* p_buffer);
  virtual ~IRProtocol();
  void do_cycle();
  void set_pipe_buffer(char* p_buffer);
  DictsMap* Proc_PyDict(PyObject* pyValue);
  void action_switch(DictsMap* dicts);
private:
  char* pipe_buffer = nullptr;
  PyThreadState *myThreadState;
  PyObject *pythonMod, *pyProc;
};
//only one class
extern IRProtocol *IRP;
