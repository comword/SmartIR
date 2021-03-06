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
#include "debug.h"

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
    DebugLog(D_WARNING,D_MAIN)<<"protocol.cpp::assert pythonMod == nullptr\n";
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
      if(strcmp(stmp, "conflicted!")==0)
        throw std::runtime_error(std::string("strcmp(stmp, \"conflicted!\")==0\n"));
      else if(strcmp(stmp, "final!")){
        PyObject* pDict = PyList_GetItem(pResult,1);
        DictsMap* m_map = Proc_PyDict(pDict);
        this->action_switch(m_map);
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
    PyObject *key = PyList_GetItem(key_dict, i);
    PyObject *value = PyDict_GetItem(pyDict, key);
    PyObject *key_bytes = PyUnicode_AsEncodedString(key, "utf-8", "strict");
    PyObject *value_bytes = PyUnicode_AsEncodedString(value, "utf-8", "strict");
    char *key_result = 0 , *value_result = 0;
    if (key_bytes != NULL && value_bytes != NULL) {
        key_result = PyBytes_AS_STRING(key_bytes); // Borrowed pointer
        key_result = strdup(key_result);
        value_result = PyBytes_AS_STRING(value_bytes); // Borrowed pointer
        value_result = strdup(value_result);
        m_map->insert(std::pair<std::string,const char*>(std::string(key_result),std::string(value_result).c_str()));
        free(key_result);
        free(value_result);
        Py_DECREF(key_bytes);
        Py_DECREF(value_bytes);
        Py_DECREF(key);
        Py_DECREF(value);
    } else {
      Py_DECREF(key_bytes);
      Py_DECREF(value_bytes);
      Py_DECREF(key);
      Py_DECREF(value);
    }
  }
  return m_map;
}
/* 1.Python class to cpp class protocol
 * a:division,b:action,c:key,d:(value)data
 * To CPP:action:{a:send ir signal,b:start learn ir signal,c:get online client}
 */
void IRProtocol::action_switch(DictsMap* dicts)
{
  char action = *((const char*)(*dicts)["b"]);
  int ID = atoi((*dicts)["c"]);
  const char *data = (const char*)(*dicts)["d"];
  int scID;
  char* tmp;
  switch (action){
    case 'a':
      scID = send_toSender(ID,0,data);
      tmp = read_fromSender(scID);
      check_sendir_result(tmp);
      free(tmp);
    break;
    case 'c':
      data = "PING";
      scID = send_toSender(255,0,data);
      tmp = read_fromSender(scID);
      free(tmp);
    break;
    case 'b':
      IR->start_learn_IR(ID);
    break;

  }
}
/*
 * To Sender Chip:action:{0:send ir signal,1:get online client}
 */
unsigned int IRProtocol::send_toSender(int ClientID,int action,const char *data)
{
  //ClientID == 255 broadcast
  if(ClientID>255 || action>255)
    throw std::runtime_error(std::string("ClientID>=255 || action>=255"));
  scID_now++;
  ClientID = ClientID & 0xff;
  action = action & 0xff;
  scID_now = scID_now & 0xffff;
  int lengthdata = strlen(data)+4;// 1 byte ID 1 byte action 2 byte scID
  char *chains = (char*)malloc(lengthdata);
  memcpy(chains,&ClientID,1);
  memcpy(chains+1,&action,1);
  memcpy(chains+2,&scID_now,2);
  memcpy(chains+4,data,lengthdata);
  put_in(chains,lengthdata);
  free(chains);
  if(scID_now >= 65535)
    scID_now = 0;
  return scID_now;
}

char *IRProtocol::read_fromSender(unsigned int scID)
{
  char *buffer = (char*)malloc(sizeof(char)*1025);
  Read_rbuffer(buffer,scID);
  return buffer;
}

char* IRProtocol::check_sendir_result(char* datas)
{
  if(strcmp(datas,"SUCCESS")==0){
    return (char*)0;
  } else {//Error
    DebugLog(D_WARNING,D_MODS)<<"IRProtocol::check_sendir_result returned a error"<<datas<<"\n";
    return datas;
  }
}
