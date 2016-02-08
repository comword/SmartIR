/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "protocol.h"
#include "pylinker.h"

#include <iostream>

IRProtocol::IRProtocol(char* p_buffer) :
transmit(),
pipe_buffer(p_buffer)
{
  PyEval_AcquireLock();
  myThreadState = PyThreadState_New(web->get_PyInterpreterState());
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
    PyThreadState_Swap(NULL);
    PyEval_ReleaseLock();
  }
  transmit::do_cycle();
}
