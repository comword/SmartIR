/* Copyright 2016 Henorvell Ge
*
* This file is a part of CampusExpBot
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "golinker.h"

#include <fstream>
#include <dlfcn.h>
#include <stdexcept>
#include <string>
golinker::golinker(const char* go_so_path)
{
  std::string exp;
	std::ifstream fin(go_so_path);
	if (!fin){
		exp="golinker.cpp:INIT Failed: No such file or directory.";
		throw std::runtime_error(exp);
		return;
	}
  goso_handle=dlopen(go_so_path, RTLD_LAZY);
	if (goso_handle == nullptr){
		exp=std::string("golinker.cpp:dlopen failed:")+dlerror();
		throw std::runtime_error(exp);
		return;
	}
	dlerror();
  InitBinding = (char* (*) (char*))dlsym(goso_handle, "InitBinding");
}
golinker::~golinker()
{

}
