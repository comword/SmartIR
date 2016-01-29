/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "transmit.h"

class IRProtocol : public transmit
{
public:
  IRProtocol();
  virtual ~IRProtocol();
  void do_cycle();
};
//only one class
extern IRProtocol *IRP;
