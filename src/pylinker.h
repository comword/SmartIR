/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include <python2.7/Python.h>

class xml_helper;
class pylinker
{
public:
  pylinker();
  //PyThreadState *get_myThreadState();
  static void * run_thread(void *ptr);
  virtual ~pylinker();
private:
  PyObject *pythonMod = nullptr;
  PyObject *StartWeb = nullptr;
  PyThreadState *PyThstate;
  PyThreadState *myThreadState;
  PyInterpreterState* interpreterState;
};
