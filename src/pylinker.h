/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include <python3.5/Python.h>
#include <string>

class xml_helper;
class pylinker
{
public:
  pylinker();
  //PyThreadState *get_myThreadState();
  static void * run_thread(void *ptr);
  int m_read_pipe(char* readbuf,int size);
  int m_write_pipe(char* writebuf,int size);
  virtual ~pylinker();
  PyInterpreterState* get_PyInterpreterState();
  void write_database(std::string dbname,std::string key,std::string value);
  std::string read_database(std::string dbname,std::string key);
  int write_IR_detail(int IRID,std::string value);
  int m_read_pipe_pre(char* readbuf);//one line pre read request
private:
  PyObject *pythonMod = nullptr;
  PyObject *StartWeb = nullptr;
  PyThreadState *PyThstate;
  PyThreadState *myThreadState;
  PyInterpreterState* interpreterState;
  int m_pipe_rpy[2];
  int m_pipe_tpy[2];
};
//only one class
extern pylinker *web;
