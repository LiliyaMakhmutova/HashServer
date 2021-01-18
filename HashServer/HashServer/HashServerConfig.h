#pragma once

struct HashServerConfig {
  std::string dir;
  std::string ip;
  size_t port;
  size_t maxtblsz; 
  size_t ntables;
  size_t workers;
  bool verbose;
};