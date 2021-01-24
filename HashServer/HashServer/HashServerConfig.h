#pragma once

/*
    dir         - Path to the directory where files will be stored (future)
    ip          - IP address of server listener
    port        - Port of server listener
    maxtblsz    - Max size of hash table (records)
    ntables     - Max number of available hash tables
    workers     - Number of threads
    verbose     - Flag that indicates that debug messages is printed to stdout
                  (stderr), if not set server prints only errors help Print help string
*/
struct HashServerConfig {
  std::string dir;
  std::string ip;
  size_t port;
  size_t maxtblsz;
  size_t ntables;
  size_t workers;
  bool verbose;
};
