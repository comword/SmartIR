/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "protocol.h"
#include "pylinker.h"
#include "IRReader.h"

#include <stdexcept>
#include <stdlib.h>

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
      PyObject* ptmp = PyList_GetItem(pResult,0);
      const char* stmp = "";
      PyArg_Parse(ptmp, "s", stmp);
      if(strcmp(stmp, "conflicted!"))
        throw std::runtime_error(std::string("strcmp(stmp, \"conflicted!\")\n"));
      else if(strcmp(stmp, "final!")){
        PyObject* pDict = PyList_GetItem(pResult,1);
        DictsMap* m_map = Proc_PyDict(pDict);
        delete m_map;
      }
    }
    Py_DECREF(arglist);
    PyThreadState_Swap(NULL);
    PyEval_ReleaseLock();
  }
  transmit::do_cycle();
}
//convert python dictionary to cpp std::map
DictsMap* IRProtocol::Proc_PyDict(PyObject* pyDict)
{
  DictsMap* m_map = new DictsMap;
  PyObject *key_dict = PyDict_Keys(pyDict);
  Py_ssize_t len = PyDict_Size(pyDict);
  for(Py_ssize_t i=0; i<len; ++i){
    PyObject* key = PyList_GetItem(key_dict, i);
    PyObject *value = PyDict_GetItem(pyDict, key);
    m_map->insert(std::pair<std::string,std::string>(PyString_AsString(key),PyString_AsString(value)));
  }
  return m_map;
}
/* 1.Python class to cpp class protocol
 * a:division,b:action,c:key,d:(value)data
 * To CPP:action:{a:send ir signal,b:start learn ir signal,c:get online client}
 */
void IRProtocol::action_switch(DictsMap* dicts)
{
  char action = *((*dicts)["b"].c_str());
  switch (action){
    case 'a':
    break;
    case 'c':
    break;
    case 'b':
      int IRID = atoi((*dicts)["c"].c_str());
      IR->start_learn_IR(IRID);
    break;
  }
}
