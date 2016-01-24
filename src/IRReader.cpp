/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "IRReader.h"

#include <fstream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdexcept>
IRReader::IRReader(const char* wiringPi_path)
{
  std::string exp;
	std::ifstream fin(wiringPi_path);
  if (!fin){
		exp="IRReader.cpp:wiringPi INIT Failed: No such file or directory.";
		throw std::runtime_error(exp);
		return;
	}
  wiringPi_handle=dlopen(wiringPi_path, RTLD_LAZY);
  if (wiringPi_handle == nullptr){
		exp=std::string("IRReader.cpp:dlopen failed:")+dlerror();
		throw std::runtime_error(exp);
		return;
	}
  dlerror();
  wiringPiSetup = (int(*)())dlsym(wiringPi_handle, "wiringPiSetup"); 
  if(wiringPiSetup () == -1){
		exp=std::string("IRReader.cpp:assert wiringPiSetup () == -1:")+strerror(errno);
		dlclose(wiringPi_handle);
		throw std::runtime_error(exp);
		return;
	}
}
IRReader::~IRReader()
{

}
