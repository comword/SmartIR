/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#pragma once
#include <string>
#include <vector>
#include <map>

#ifdef _MSC_VER
#   include <Windows.h>
#   include "wdirent.h"
#   include <direct.h>
#else
#   include <dirent.h>
#   include <unistd.h>
#endif

typedef std::map<std::string,std::string> FILEMAPCL;

namespace PATH_CLASS
{
  unsigned long get_file_size(const char *path);
  bool do_mkdir(std::string const& path, int const mode);
  bool assure_dir_exist( std::string const &path );
  bool file_exist( const std::string &path );
  bool remove_file( const std::string &path );
  bool rename_file( const std::string &old_path, const std::string &new_path );
  bool is_directory(dirent const &entry, std::string const &full_path);
  void init_user_dir( const char *ud = "" );
  void update_datadir( void );
  void update_pathname( std::string name, std::string path );
  std::string get_pathname(std::string name);
  void check_logs();
  void GetFilesInDirectory(std::vector<std::string> &out, const std::string &directory);
}
