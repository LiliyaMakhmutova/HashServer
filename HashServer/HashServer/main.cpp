#define _CRT_SECURE_NO_WARNINGS // avoid some unsafe functions from getopt.h error
#include "HashServer.h"

#define WIN32_LEAN_AND_MEAN // boost on windows

#include <stdio.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "HashServer.h"
#include "HashServerConfig.h"
#include "getopt.h" /* getopt_long */

bool help_opt = false;

/** \brief Parses console parameters
 * \param[in] argc number of arguments
 * \param[in] argv string arguments
 * \param[out] config stores parsed configuration
 *
 * This method parses console parameters. Uses getopt linux lib for getopt_long function.
 * The parameters are: 
 *  -d --dir=<path>
 *  -i --ip=<IP> 
 *  -p --port=<uint>
 *  -m --maxtblsz=<uint>
 *  -n --ntables=<uint>
 *  -w --workers=<uint>
 *  -v --verbose
 *  -h --help
 *
 */
void parse_console_parameters(int argc, char **argv, HashServerConfig &config);


int main(int argc, char **argv) {
  /*HashServerConfig config;
  parse_console_parameters(argc, argv, config);

  std::cout << config.dir << " " << config.ip << " " << config.maxtblsz << " "
            << config.ntables << " " << config.port << " " << config.verbose
            << " " << config.workers << endl;
            
  */
  HashServerConfig config; // all console config parameters are stored here
  config.ip = "127.0.0.1";
  config.port = 1234;
  config.workers = 8;
  if (!help_opt) {
    try {
      boost::thread_group threads_; // thread_pool
      boost::asio::io_context io_context;
      boost::asio::io_context::work work_(io_context);
      
      // setting thread_pool tasks
      for (std::size_t i = 0; i < config.workers; ++i) {
        threads_.create_thread(
            boost::bind(&boost::asio::io_context::run, &io_context));
      } 
      HashServer server(io_context, config); // create server and run it
      io_context.run();
    } catch (std::exception &e) {
      std::cerr << e.what() << endl;
    }
  }
  return 0;
}


void parse_console_parameters(int argc, char **argv, HashServerConfig &config) {
  static struct option long_options[] = {
      {"dir", required_argument, 0, 'd'},
      {"ip", required_argument, 0, 'i'},
      {"port", required_argument, 0, 'p'},
      {"maxtblsz", required_argument, 0, 'm'},
      {"ntables", required_argument, 0, 'n'},
      {"workers", required_argument, 0, 'w'},
      {"verbose", no_argument, 0, 'v'},  // 0
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int c, option_index = 0;
  while (-1 != (c = getopt_long(argc, argv, "d:i:p:m:n:w:v:h", long_options,
                                &option_index))) {
    switch (c) {
      case 0:
        printf("option %s", long_options[option_index].name);
        if (optarg) printf(" with arg %s", optarg);
        printf("\n");
        switch (option_index) {
          case 0:
            config.dir = optarg;
            break;
          case 1:
            config.ip = optarg;
            break;
          case 2:
            config.port = std::stoi(optarg);
            break;
          case 3:
            config.maxtblsz = std::stoi(optarg);
            break;
          case 4:
            config.ntables = std::stoi(optarg);
            break;
          case 5:
            config.workers = std::stoi(optarg);
            break;
          case 6:
            config.verbose = true;  // optarg;
            break;
          case 7:
            help_opt = true;
            printf(
                "using:\n\t./exe -d|--dir <dir> -i|--ip <ip> -p|--port <port> "
                "-m|--maxtblsz <uint> -n|--ntables <uint> -w|--workers <num> "
                "[-v|--verbose ] [-h|--help <uint>]\n\n");
            break;
          case 8:
            printf(
                "using:\n\t./exe -d|--dir <dir> -i|--ip <ip> -p|--port <port> "
                "-m|--maxtblsz <uint> -n|--ntables <uint> -w|--workers <num> "
                "[-v|--verbose ] [-h|--help <uint>]\n\n");
            break;
        }
        break;

      case 'd':
        config.dir = optarg;
        break;
      case 'i':
        config.ip = optarg;
        break;
      case 'p':
        config.port = std::stoi(optarg);
        break;
      case 'm':
        config.maxtblsz = std::stoi(optarg);
        break;
      case 'n':
        config.ntables = std::stoi(optarg);
        break;
      case 'w':
        config.workers = std::stoi(optarg);
        break;
      case 'v':
        config.verbose = std::stoi(optarg);
        break;
      case 'h':
        help_opt = true;
        printf(
            "using:\n\t./exe -d|--dir <dir> -i|--ip <ip> -p|--port <port> "
            "-m|--maxtblsz <uint> -n|--ntables <uint> -w|--workers <num> "
            "[-v|--verbose <uint>] [-h|--help <uint>]\n\n");
        break;

      case '?': /* getopt_long already printed an error message. */
        printf(
            "using:\n\t./exe -d|--dir <dir> -i|--ip <ip> -p|--port <port> "
            "-m|--maxtblsz <uint> -n|--ntables <uint> -w|--workers <num> "
            "[-v|--verbose <uint>] [-h|--help <uint>]\n\n");
        break;

      default:
        printf("?? getopt returned character code 0%o ??\n", c);
        break;
    }
  }
  if (optind < argc) {
    printf("non-option argv elements: ");
    while (optind < argc) printf("%s ", argv[optind++]);
    printf("\n");
  }
}