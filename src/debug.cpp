/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "debug.h"
#include "mfilesystem.h"

#include <fcntl.h>
#include <time.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
//#include <cerrno>
#include <memory>
#include <sys/stat.h>
#include <exception>
#include <errno.h>
#include <limits.h>

#ifndef _MSC_VER
#include <sys/time.h>
#endif

#if !(defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
#include <execinfo.h>
#include <stdlib.h>
#endif

//#if (defined(DEBUG) || defined(_DEBUG)) && !defined(NDEBUG)
static int debugLevel = DL_ALL;
static int debugClass = DC_ALL;
//#else
//static int debugLevel = D_ERROR;
//static int debugClass = D_MAIN;
//#endif

#define TRACE_SIZE 20
void *tracePtrs[TRACE_SIZE];
bool debug_fatal = false;
bool debug_mode = false;
// Null OStream                                                     {{{2
// ---------------------------------------------------------------------

struct NullBuf : public std::streambuf {
  NullBuf() {}
  int overflow( int c ) override {
    return c;
  }
};

static NullBuf nullBuf;
static std::ostream nullStream( &nullBuf );

std::string currentTime()
{
  time_t t = time(0);
  char tmp[25];
  strftime(tmp, sizeof(tmp),"[%Y-%m-%d %H:%M:%S]",localtime(&t));
  return std::string( tmp );
}
/*
std::ofstream &currentLevel(DebugLevel level)
{
return ( std::cout << "[]" );
}
*/
void limitDebugLevel( int level_bitmask )
{
  DebugLog( DL_ALL, DC_ALL ) << "Set debug level to: " << level_bitmask;
  debugLevel = level_bitmask;
}

void limitDebugClass( int class_bitmask )
{
  DebugLog( DL_ALL, DC_ALL ) << "Set debug class to: " << class_bitmask;
  debugClass = class_bitmask;
}

std::ostream &operator<<( std::ostream &out, DebugLevel lev )
{
  if( lev != DL_ALL ) {
    if( lev & D_INFO ) {
      out << "INFO";
    }
    if( lev & D_WARNING ) {
      out << "WARNING";
    }
    if( lev & D_ERROR ) {
      out << "ERROR";
    }
  }
  return out;
}

void init_log()
{
  std::cout << "-----------------------------------------\n";
  std::cerr << "-----------------------------------------\n";
  std::string m_time = currentTime();
  std::cout << m_time <<" : Starting log.\n";
  std::cerr << m_time <<" : Starting log.\n";
}

std::ostream &operator<<( std::ostream &out, DebugClass cl )
{
  if( cl != DC_ALL ) {
    if( cl & D_MAIN ) {
      out << "MAIN";
    }
    if( cl & D_MODS ) {
      out << "MODS";
    }
    if( cl & D_NETWORK ) {
      out << "NETWORK";
    }
    if( cl & D_PYTHON ) {
      out << "PYTHON";
    }
  }
  return out;
}

std::ostream &DebugLog( DebugLevel lev, DebugClass cl )
{
  // Error are always logged, they are important.
  if( ( ( lev & debugLevel ) && ( cl & debugClass ) ) || lev & D_ERROR ) {
    if(lev == D_ERROR){
      std::string m_time = currentTime();
      //std::cerr << std::endl;
      std::cerr << m_time<< " ";
      std::cerr << "["<< lev << "] ";
      std::cerr << "["<< cl << "]";
      std::cerr << ": ";
#if !(defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
      int count = backtrace( tracePtrs, TRACE_SIZE );
      char **funcNames = backtrace_symbols( tracePtrs, count );
      for( int i = 0; i < count; ++i ) {
        std::cerr << "\n\t(" << funcNames[i] << "), ";
      }
      std::cerr << "\n\t";
      free( funcNames );
#endif
      return std::cerr;
    } else {
      //std::cout << std::endl;
      std::string m_time = currentTime();
      std::cout << m_time<< " ";
      std::cout << "["<< lev << "] ";
      std::cout << "["<< cl << "]";
      std::cout << ": ";
      return std::cout;
    }
  }
  return nullStream;
}

void redirect_out_to_log(bool screen)
{
  using namespace PATH_CLASS;
  std::string dir = get_pathname("logdir");
  if( dir=="NULL" ){
    return;
  } else if(!screen) {
    std::string logerror = dir + std::string("error.log");
    std::string logaccess = dir + std::string("access.log");
    freopen(logaccess.c_str(), "w",stdout);
    freopen(logerror.c_str(), "w",stderr);
  } else {
#if !(defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
    int *out_p = (int *)malloc(3*sizeof(int));
    int *err_p = (int *)malloc(3*sizeof(int));
    int logger_acc[2],logger_err[2];
    pipe(logger_acc);
    pipe(logger_err);
    out_p[0] = logger_acc[0];
    err_p[0] = logger_err[0];
    out_p[2] = dup(STDOUT_FILENO);
    err_p[2] = dup(STDOUT_FILENO);
    std::string logerror = dir + std::string("error.log");
    err_p[1] = open(logerror.c_str(),O_WRONLY|O_APPEND|O_CREAT,0655);
    std::string logaccess = dir + std::string("access.log");
    out_p[1] = open(logaccess.c_str(),O_WRONLY|O_APPEND|O_CREAT,0655);
    pthread_attr_t attr;
    pthread_t tid_out,tid_err;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    dup2(logger_err[1], STDERR_FILENO);
    dup2(logger_acc[1], STDOUT_FILENO);
    pthread_create(&tid_out, &attr, log_thread, out_p);
    pthread_create(&tid_err, &attr, log_thread, err_p);
    close(logger_err[1]);
    close(logger_acc[1]);
#endif
  }
}
#if !(defined _WIN32 || defined WINDOWS || defined __CYGWIN__)
void * log_thread(void *ptr)
{
  int *p = (int *)ptr;
  char buffer[4096];
  int reada;
  while ((reada = read(p[0], buffer, sizeof(buffer))) != 0)
  {
    int nwritten_total = 0;
    int nwritten = 0;
    while (nwritten_total < reada)
    {
      nwritten = write(p[1], buffer + nwritten_total, reada - nwritten_total);
      nwritten_total += nwritten;
    }
    nwritten_total = 0;
    nwritten = 0;
    while (nwritten_total < reada)
    {
      nwritten = write(p[2],buffer + nwritten_total, reada - nwritten_total);
      nwritten_total += nwritten;
    }
  }
  return 0;
}
#endif
