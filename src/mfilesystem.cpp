/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "mfilesystem.h"

#include <stdexcept>
#include <string.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <memory>
#include <time.h>

#if (defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
#include <windows.h>
#endif

namespace PATH_CLASS
{
  FILEMAPCL FILENAMES;
  unsigned long get_file_size(const char *path)
  {
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
      return filesize;
    } else {
      filesize = statbuff.st_size;
    }
    return filesize;
  }
#if (defined _WIN32 || defined __WIN32__)
  bool do_mkdir(std::string const& path, int const mode)
  {
    (void)mode; //not used on windows
    return _mkdir(path.c_str()) == 0;
  }
#else
  bool do_mkdir(std::string const& path, int const mode)
  {
    return mkdir(path.c_str(), mode) == 0;
  }
#endif
  bool assure_dir_exist(std::string const &path)
  {
    std::unique_ptr<DIR, decltype(&closedir)> dir {opendir(path.c_str()), closedir};
    return dir || do_mkdir(path, 0755);
  }

  bool file_exist(const std::string &path)
  {
    struct stat buffer;
    return ( stat( path.c_str(), &buffer ) == 0 );
  }
#if (defined _WIN32 || defined __WIN32__)
  bool remove_file(const std::string &path)
  {
	int Len = ::MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, NULL, 0);
	LPWSTR *buf = new LPWSTR[Len];
	MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, *buf, Len);
	if (DeleteFile(*buf) != 0) {
		delete buf;
		return true;
	}
	delete buf;
    return false;
  }
#else
  bool remove_file(const std::string &path)
  {
    return unlink(path.c_str()) == 0;
  }
#endif

#if (defined _WIN32 || defined __WIN32__)
  bool rename_file(const std::string &old_path, const std::string &new_path)
  {
    // Windows rename function does not override existing targets, so we
    // have to remove the target to make it compatible with the linux rename
    if (file_exist(new_path)) {
      if(!remove_file(new_path)) {
        return false;
      }
    }
    return rename(old_path.c_str(), new_path.c_str()) == 0;
  }
#else
  bool rename_file(const std::string &old_path, const std::string &new_path)
  {
    return rename(old_path.c_str(), new_path.c_str()) == 0;
  }
#endif
bool is_directory_stat(std::string const &full_path)
{
    struct stat result;
    if (stat(full_path.c_str(), &result) != 0) {
        auto const e_str = strerror(errno);
        throw std::runtime_error(std::string("stat [")+full_path+std::string("] failed with \"")+e_str+"\".");
        return false;
    }

    return S_ISDIR(result.st_mode);
}
#if defined (__MINGW32__)
  bool is_directory(dirent const &entry, std::string const &full_path)
  {
    // no dirent::d_type
    (void)entry; //not used for mingw
    return is_directory_stat(full_path);
  }
#else
  bool is_directory(dirent const &entry, std::string const &full_path)
  {
    if (entry.d_type == DT_DIR) {
      return true;
    } else if (entry.d_type != DT_UNKNOWN) {
      return false;
    }

    return is_directory_stat(full_path);
  }
#endif
  void init_user_dir( const char *ud )
  {
    char * cwd;
#if (defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
    if ((cwd = _getcwd(nullptr,0)) == nullptr)
#else
    if ((cwd = getcwd(nullptr,0)) == nullptr)
#endif
      throw std::runtime_error(std::string("mfilesystem.cpp:getcwd() Failed!\n"));
    std::string bdir = std::string(cwd);
    char ch;
    ch = bdir.at(bdir.length() - 1);
    if (ch != '/' || ch != '\\') {
#if !(defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
		bdir.push_back('/');
#else
		bdir.push_back('\\');
#endif
    }
    free(cwd);
    FILENAMES["basedir"]=bdir;
    std::string udir = std::string(ud);
    if(udir.empty()){
#if !(defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
      const char *user_dir = getenv("HOME");
#else
	  const char *user_dir = getenv("LOCALAPPDATA");
#endif
      udir = std::string(user_dir) + "/InsurGameServer/";
    }
    assure_dir_exist(udir);
    FILENAMES["userdir"]=udir;
  }
  void update_datadir( void )
  {
    assure_dir_exist(FILENAMES["userdir"] + "datas");
    update_pathname("datadir", FILENAMES["userdir"] + "datas");
    assure_dir_exist(FILENAMES["userdir"] + "logs");
    update_pathname("logdir", FILENAMES["userdir"] + "logs");
    assure_dir_exist(FILENAMES["baseadir"] + "python");
    update_pathname("pythondir", FILENAMES["basedir"] + "python");
    assure_dir_exist(FILENAMES["basedir"] + "modules");
    update_pathname("librarydir", FILENAMES["basedir"] + "modules");
    assure_dir_exist(FILENAMES["datadir"] + "databases");
    update_pathname("databasedir", FILENAMES["datadir"] + "databases");
  }
  void update_pathname( std::string name, std::string path )
  {
    char ch;
    ch = path.at(path.length() - 1);
    if (ch != '/' || ch != '\\') {
#if !(defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
      path.push_back('/');
#else
	  path.push_back('\\');
#endif
    }
    FILEMAPCL::iterator iter;
    iter = FILENAMES.find(name);
    if (iter != FILENAMES.end()) {
        FILENAMES[name] = path;
    } else {
        FILENAMES.insert(std::pair<std::string, std::string>(name, path));
    }
  }
  std::string get_pathname(std::string name)
  {
    return FILENAMES[name];
  }
  void check_logs()
  {
    using namespace std;
    string dir = FILENAMES["logdir"];
    vector<string> files;
    GetFilesInDirectory(files,dir);
    vector<string>::iterator iter;
    iter = files.begin();
    for (;iter!=files.end();iter++){
      string s_path = *iter;
      time_t t = time(0);
      char tmp[15];
      strftime(tmp, sizeof(tmp),"%Y%m%d%H%M%S",localtime(&t));
      if (get_file_size(s_path.c_str()) >= 1000000) //current log too large move it
        if (s_path.substr(s_path.length() - 4,4) == ".log")
        rename_file(s_path,s_path+"."+tmp);
    }
  }

  void GetFilesInDirectory(std::vector<std::string> &out, const std::string &directory)
  {
#if (defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
    HANDLE dir;
    WIN32_FIND_DATAA file_data;
    if ((dir = FindFirstFileA((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
      return; /* No files found */
    do {
      char ch;
      ch = directory.at(directory.length() - 1);
      std::string full_file_name;
      const std::string file_name = file_data.cFileName;
      if (ch == '\\')
        full_file_name = directory + file_name;
      else
        full_file_name = directory + "\\" + file_name;
      const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

      if (file_name[0] == '.')
      continue;

      if (is_directory)
      continue;

      out.push_back(full_file_name);
    } while (FindNextFileA(dir, &file_data));

    FindClose(dir);
#else
    DIR *dir;
    class dirent *ent;
    class stat st;
    dir = opendir(directory.c_str());
    while ((ent = readdir(dir)) != NULL) {
      const std::string file_name = ent->d_name;
      char ch;
      ch = directory.at(directory.length() - 1);
      std::string full_file_name;
      if (ch == '/')
        full_file_name = directory + file_name;
      else
        full_file_name = directory + '/' + file_name;
      if (file_name[0] == '.')
      continue;

      if (stat(full_file_name.c_str(), &st) == -1)
      continue;

      const bool is_directory = (st.st_mode & S_IFDIR) != 0;

      if (is_directory)
      continue;

      out.push_back(full_file_name);
    }
    closedir(dir);
#endif
  }
}
