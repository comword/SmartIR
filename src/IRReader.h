/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "wiringPi.h"

class IRReader
{
public:
  IRReader(const char* wiringPi_path);
  virtual ~IRReader();
  int (*wiringPiSetupSys) ();
  int (*wiringPiISR) (int pin, int edgeType,  void (*function)(void)) ;
  int (*piThreadCreate) (void *(*fn)(void *));
  int (*piHiPri) (int pri);
  int (*waitForInterrupt) (int pin, int mS);
  int gpio_set_edge(unsigned int gpio, char *edge);
  static void *wait_for_IR (void * ptr);
private:
  void *wiringPi_handle;
};
extern IRReader *IR;
