/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "transmit.h"

#include <python2.7/Python.h>

class IRProtocol : public transmit
{
public:
  IRProtocol(char* p_buffer);
  virtual ~IRProtocol();
  void do_cycle();
  void set_pipe_buffer(char* p_buffer);
private:
  char* pipe_buffer = nullptr;
  PyThreadState *myThreadState;
};
//only one class
extern IRProtocol *IRP;
