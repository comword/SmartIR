/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "protocol.h"
#include "pylinker.h"

#include <stdexcept>
#include <string>

IRProtocol::IRProtocol(char* p_buffer) :
transmit(),
pipe_buffer(p_buffer)
{
  PyEval_AcquireLock();
  myThreadState = PyThreadState_New(web->get_PyInterpreterState());
  PyThreadState_Swap(myThreadState);
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append('./python')");
  PyImport_ImportModule("jsontransfer");
  if (pythonMod == nullptr)
    throw std::runtime_error(std::string("protocol.cpp::assert pythonMod == nullptr\n"));
  pyProc = PyObject_GetAttrString(pythonMod, "transjson");
  PyEval_ReleaseLock();
}
IRProtocol::~IRProtocol()
{
  PyEval_AcquireLock();
  PyThreadState_Swap(NULL);
  PyThreadState_Clear(myThreadState);
  PyThreadState_Delete(myThreadState);
  PyEval_ReleaseLock();
}
void IRProtocol::do_cycle()
{
  if (pipe_buffer != nullptr){
    PyEval_AcquireLock();
    PyThreadState_Swap(myThreadState);
    PyObject *arglist,*pResult;
    arglist = Py_BuildValue("(s)", pipe_buffer);
    pResult = PyEval_CallObject(pyProc,arglist);
    if (pResult != nullptr){
      
    }
    Py_DECREF(arglist);
    PyThreadState_Swap(NULL);
    PyEval_ReleaseLock();
  }
  transmit::do_cycle();
}
