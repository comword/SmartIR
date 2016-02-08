/* Copyright 2016 Henorvell Ge
*
* This file is a part of SmartIR
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "pylinker.h"
#include "IRReader.h"
#include "protocol.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <functional>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdexcept>
#include <map>
#include <iostream>

pylinker *web;
IRReader *IR;
IRProtocol *IRP;
void exit_handler(int s);

namespace {

  struct arg_handler {
    //! Handler function to be invoked when this argument is encountered. The handler will be
    //! called with the number of parameters after the flag was encountered, along with the array
    //! of following parameters. It must return an integer indicating how many parameters were
    //! consumed by the call or -1 to indicate that a required argument was missing.
    typedef std::function<int(int, const char **)> handler_method;

    const char *flag;  //!< The commandline parameter to handle (e.g., "--seed").
    const char *param_documentation;  //!< Human readable description of this arguments parameter.
    const char *documentation;  //!< Human readable documentation for this argument.
    const char *help_group; //!< Section of the help message in which to include this argument.
    handler_method handler;  //!< The callback to be invoked when this argument is encountered.
  };

  void printHelpMessage(const arg_handler *arguments, size_t num_arguments);
}  // namespace

int main(int argc, char *argv[])
{
  bool if_exit = false;
  bool is_daemon = false;
  //  const char *help_section_default = nullptr;
  const char *help_section_system = "System";
  const arg_handler arg_proc[]={
    {
      "--daemon",nullptr,"Start program background.",
      help_section_system,
      [&is_daemon](int, const char **) -> int {
        is_daemon = true;
        return 0;
      }
    }
  };
  // Process CLI arguments.
  const size_t num_arguments =
  sizeof(arg_proc) / sizeof(arg_proc[0]);
  //int saved_argc = --argc; // skip program name
  //const char **saved_argv = (const char **)++argv;
  --argc;
  ++argv;
  while (argc) {
    if(!strcmp(argv[0], "--help")) {
      printHelpMessage(arg_proc, num_arguments);
      return 0;
    } else {
      bool arg_handled = false;
      for (size_t i = 0; i < num_arguments; ++i) {
        auto &arg_handler = arg_proc[i];
        if (!strcmp(argv[0], arg_handler.flag)) {
          argc--;
          argv++;
          int args_consumed = arg_handler.handler(argc, (const char **)argv);
          if (args_consumed < 0) {
            printf("Failed parsing parameter '%s'\n", *(argv - 1));
            exit(1);
          }
          argc -= args_consumed;
          argv += args_consumed;
          arg_handled = true;
          break;
        }
      }
      // Skip other options.
      if (!arg_handled) {
        --argc;
        ++argv;
      }
    }
  }
  //Start INIT
  if (is_daemon == true) {
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
      exit(EXIT_FAILURE);
    }
    if (pid > 0) {
      exit(EXIT_SUCCESS);
    }
    umask(0);
    sid = setsid();
    if (sid < 0) {
      exit(EXIT_FAILURE);
    }
    freopen("/dev/null", "w",stdout);
    freopen("/dev/null", "w",stderr);
    //close(STDIN_FILENO);
    //close(STDOUT_FILENO);
    //close(STDERR_FILENO);
  }
//  std::system("clear"); // Clear screen
  struct sigaction sigHandler;
  sigHandler.sa_handler = exit_handler;
  sigemptyset(&sigHandler.sa_mask);
  sigHandler.sa_flags = 0;
  sigaction(SIGINT, &sigHandler, NULL);
  try {
    web = new pylinker();
  }
  catch (std::runtime_error &e) {
    std::cerr<<e.what()<<std::endl;
    exit_handler(999);
  }
  char *pipe_buffer = (char *)malloc(1024*sizeof(char));
  try {
    IR = new IRReader();
  }
  catch (std::runtime_error &e) {
    std::cerr<<e.what()<<std::endl;
    std::cerr<<"IR module won't work!"<<std::endl;
  }
  try {
    IRP = new IRProtocol(pipe_buffer);
  }
  catch (std::runtime_error &e) {
    std::cerr<<e.what()<<std::endl;
    std::cerr<<"IR module won't work!"<<std::endl;
  }
  do {
    sleep(1);
    web->m_read_pipe(pipe_buffer,sizeof(pipe_buffer));
    IRP->do_cycle();
  } while (!if_exit);
}

namespace {
  void printHelpMessage(const arg_handler *arguments,size_t num_arguments)
  {
    // Group all arguments by help_group.
    std::multimap<std::string, const arg_handler *> help_map;
    for (size_t i = 0; i < num_arguments; ++i) {
      std::string help_group;
      if( arguments[i].help_group ) {
        help_group = arguments[i].help_group;
      }
      help_map.insert( std::make_pair(help_group, &arguments[i]) );
    }
    printf("Command line paramters:\n");
    std::string current_help_group;
    auto it = help_map.begin();
    auto it_end = help_map.end();
    for (; it != it_end; ++it) {
      if (it->first != current_help_group) {
        current_help_group = it->first;
        printf("\n%s\n", current_help_group.c_str());
      }
      const arg_handler *handler = it->second;
      printf("%s", handler->flag);
      if (handler->param_documentation) {
        printf(" %s", handler->param_documentation);
      }
      printf("\n");
      if (handler->documentation) {
        printf("\t%s\n", handler->documentation);
      }
    }
  }
}  // namespace
void exit_handler(int n)
{
  if (n == 2){
//    std::system("clear");
    int status = 0;
    std::exit(status);
  }
  else
  std::exit(n);
}
