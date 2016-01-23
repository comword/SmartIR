/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "pylinker.h"

#include <stdexcept>
#include <string>
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
  int res = pthread_create(&tid, NULL, pylinker::run_thread, this);
  if (res)
    throw std::runtime_error(std::string("pylinker::pthread_create() Failed!\n"));
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
  PyEval_CallObject(orgclass->StartWeb,nullptr);
  PyThreadState_Swap(NULL);
  PyEval_ReleaseLock();
  return 0;
}
pylinker::~pylinker()
{
  PyEval_AcquireLock();
  PyThreadState_Swap(NULL);
  PyThreadState_Clear(PyThstate);
  PyThreadState_Delete(PyThstate);
  Py_Finalize();
}