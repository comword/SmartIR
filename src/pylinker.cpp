/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "pylinker.h"

#include <stdexcept>
#include <pthread.h>
#include <unistd.h>
#include <iostream>

pylinker::pylinker()
{
  Py_Initialize();
  PyEval_InitThreads();
  if (!Py_IsInitialized())
  throw std::runtime_error(std::string("pylinker.cpp::assert !Py_IsInitialized()\n"));
  PyThstate = PyThreadState_Get();
  interpreterState = PyThstate->interp;
  PyEval_ReleaseLock();
  pthread_attr_t attr;
  pthread_t tid;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  int res = pthread_create(&tid, &attr, pylinker::run_thread, this);
  if (res)
    throw std::runtime_error(std::string("pylinker::pthread_create() Failed!\n"));
  if(pipe(m_pipe) < 0)
    throw std::runtime_error(std::string("pylinker::assert pipe(m_pipe) < 0\n"));
}
void * pylinker::run_thread(void *ptr)
{
  pylinker *orgclass = (pylinker *) ptr;
  PyEval_AcquireLock();
  orgclass->myThreadState = PyThreadState_New(orgclass->interpreterState);
  PyThreadState_Swap(orgclass->myThreadState);
  PyRun_SimpleString("import sys\n");
  PyRun_SimpleString("sys.path.append('./python')");
  orgclass->pythonMod = PyImport_ImportModule("main");
  if (orgclass->pythonMod == nullptr)
    throw std::runtime_error(std::string("pylinker.cpp::assert pythonMod == nullptr\n"));
  orgclass->StartWeb = PyObject_GetAttrString(orgclass->pythonMod, "startWebServer");
  PyObject *arglist;
  arglist = Py_BuildValue("(i)", orgclass->m_pipe[1]);
  PyEval_CallObject(orgclass->StartWeb,arglist);
  Py_DECREF(arglist);
  PyThreadState_Swap(NULL);
  PyEval_ReleaseLock();
  return 0;
}
pylinker::~pylinker()
{
  PyEval_AcquireLock();
  PyThreadState_Swap(NULL);
  PyThreadState_Clear(myThreadState);
  PyThreadState_Clear(myThreadState);
  PyThreadState_Clear(PyThstate);
  PyThreadState_Delete(PyThstate);
  Py_Finalize();
}
int pylinker::m_read_pipe(char* readbuf,int size)
{
  return read(m_pipe[0],readbuf,size);
}
int pylinker::m_write_pipe(char* writebuf,int size)
{
  return write(m_pipe[0],writebuf,size);
}
PyInterpreterState* pylinker::get_PyInterpreterState()
{
  return interpreterState;
}
void pylinker::write_database(std::string dbname,std::string key,std::string value)
{
  PyEval_AcquireLock();
  PyThreadState_Swap(myThreadState);
  PyObject *we_database,*arglist;
  we_database = PyObject_GetAttrString(pythonMod, "we_database");
  arglist = Py_BuildValue("(sss)", dbname.c_str(),key.c_str(),value.c_str());
  PyEval_CallObject(we_database,arglist);
  Py_DECREF(arglist);
  PyThreadState_Swap(NULL);
  PyEval_ReleaseLock();
}
std::string pylinker::read_database(std::string dbname,std::string key)
{
  PyEval_AcquireLock();
  PyThreadState_Swap(myThreadState);
  PyObject *rd_database,*arglist,*res;
  rd_database = PyObject_GetAttrString(pythonMod, "rd_database");
  arglist = Py_BuildValue("(ss)", dbname.c_str(),key.c_str());
  res = PyEval_CallObject(rd_database,arglist);
  const char *m_str = "";
  PyArg_Parse(res, "s", m_str);
  Py_DECREF(arglist);
  PyThreadState_Swap(NULL);
  PyEval_ReleaseLock();
  return std::string(m_str);
}
