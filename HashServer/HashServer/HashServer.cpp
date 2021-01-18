#include "HashServer.h"
#include <exception>

std::vector<Table> tables;
size_t size = 0;
std::mutex mutex_;
size_t ntables;
bool VERBOSE;


void con_handler::start() {
  socket_.async_read_some(
      boost::asio::buffer(in_message, BUFFER_SIZE),
      boost::bind(&con_handler::handle_read, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

void con_handler::handle_read(const boost::system::error_code& err,
                 size_t bytes_transferred) {
  if (!err) {
    if (VERBOSE) {
      cout << "Server recieved from client: " << in_message << endl;
    }
    socket_.async_write_some(
        boost::asio::buffer(out_message = parse_command_str(in_message),
                            BUFFER_SIZE),
        boost::bind(&con_handler::handle_write, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  } else {
    std::cerr << "error: " << err.message() << std::endl;
    socket_.close();
  }
}

void con_handler::handle_write(const boost::system::error_code& err,
                  size_t bytes_transferred) {
  if (!err) {
    if (VERBOSE) {
      cout << "Server successfully sent message to the client: " << out_message
           << endl;
    }
  } else {
    std::cerr << "error: " << err.message() << endl;
    socket_.close();
  }
}

std::string con_handler::add_table(std::string username) {
  std::lock_guard<std::mutex> lg(mutex_);
  if (VERBOSE) {
    cout << "Table number " << size << " was successfully added for user "
         << username << endl;
  }
  tables.push_back({username, HashMap(), true});
  size++;
  return std::to_string(tables.size() - 1);
}

std::string con_handler::get_table(size_t table_num) {
  std::lock_guard<std::mutex> lg(mutex_);
  if (VERBOSE) {
    cout << "Getting table with number " << table_num << endl;
  }
  return tables[table_num].hash_map.get_table();
}

void con_handler::set_val(size_t table_num, int key, std::string val,
                          time_t ttl) {
  std::lock_guard<std::mutex> lg(mutex_);
  if (VERBOSE) {
    cout << "Setting table's with number " << table_num << " key: " << key
         << " equal to value: " << val << " with ttl: " << ttl << " seconds."
         << endl;
  }
  tables[table_num].hash_map.put(key, val, ttl);
}

std::optional<std::string> con_handler::get_val(size_t table_num, int key) {
  std::lock_guard<std::mutex> lg(mutex_);
  if (VERBOSE) {
    cout << "Getting table's with number " << table_num << " key: " << key
         << endl;
  }
  return tables[table_num].hash_map.get(key);
}

std::string con_handler::remove_table(size_t table_num) {
  std::lock_guard<std::mutex> lg(mutex_);
  if (VERBOSE) {
    cout << "Removing table with number " << table_num << endl;
  }
  tables[table_num].valid = false;
  tables[table_num].hash_map.free_hash_map();
  size--;  
  if (VERBOSE) {
    cout << "Table number " << table_num << " was successfully deleted."
         << endl;
  }
  return "";
}

std::string con_handler::parse_command_str(std::string str) {
  std::istringstream ss(str);
  std::string token;

  std::string username;
  std::getline(ss, username, ' ');
  if (VERBOSE) {
    cout << "Recieved command from user: " << username << endl;
  }

  std::getline(ss, token, ' ');
  if (VERBOSE) {
    cout << "Command is " << token << ". Parsing command..." << endl;
  }
  try {
    if (token == "addtable") {
      if (ntables == size) {
        if (VERBOSE) {
          cout << "Table limit exceeded." << endl;
        }
        return get_table_error(ntables);  // too much
      } else {
        return add_table(username);
      }
    } else if (token == "remtable") {
      std::getline(ss, token, ' ');
      size_t num = std::stoi(token);
      if (is_valid_table(num)) {
        if (tables[num].username == username) {
          return remove_table(num);
        } else {
          if (VERBOSE) {
            cout << "Table number " << num << " does not belong to user "
                 << username
                 << " (this user does not have privileges to delete)." << endl;
          }
          return get_table_error(num);  // rights issues
        }
      } else {
        if (VERBOSE) {
          cout << "There is no table with number " << num << endl;
        }
        return get_table_error(num);  // no such idx
      }
    } else if (token == "gettable") {
      std::getline(ss, token, ' ');
      size_t num = std::stoi(token);
      if (is_valid_table(num)) {
        if (tables[num].username == username) {
          return get_table(num);
        } else {
          if (VERBOSE) {
            cout << "Table number " << num << " does not belong to user "
                 << username
                 << " (this user does not have privileges to get it's full "
                    "content, try get values by the keys)."
                 << endl;
          }
          return get_table_error(num);  // rights issues
        }
      } else {
        if (VERBOSE) {
          cout << "There is no table with number " << num << endl;
        }
        return get_table_error(num);  // no such idx
      }
    } else if (token == "setval") {
      std::getline(ss, token, ' ');
      size_t key = std::stoi(token.substr(4));
      std::getline(ss, token, ' ');
      std::string val = token.substr(4);
      std::getline(ss, token, ' ');
      size_t table_num = std::stoi(token.substr(6));
      std::getline(ss, token, ' ');
      size_t ttl = std::stoi(token.substr(4));

      if (is_valid_table(table_num)) {
        set_val(table_num, key, val, ttl);
        return "";
      } else {
        return get_table_error(table_num);
      }
    } else if (token == "getval") {
      std::getline(ss, token, ' ');
      size_t key = std::stoi(token.substr(4));
      std::getline(ss, token, ' ');
      size_t table_num = std::stoi(token.substr(6));

      if (is_valid_table(table_num)) {
        auto value = get_val(table_num, key);
        if (value != std::nullopt) {
          return get_okey(key, *value, table_num);
        } else {
          return get_key_error(key);
        }
      } else {
        if (VERBOSE) {
          cout << "There is no table with number " << table_num << endl;
        }
        return get_table_error(table_num);
      }
    } else {
      if (VERBOSE) {
        cout << "Unknown command" << endl;
      }
    }
  } catch (std::exception& err) {
    cout << "Request parsing failed: " << err.what() << endl;
  }
  return "";
}

bool con_handler::is_valid_table(size_t table_num) {
  std::lock_guard<std::mutex> lg(mutex_);
  if (VERBOSE) {
    cout << "Checking whether table is valid..." << endl;
  }  
  return tables.size() > table_num && tables[table_num].valid;
}

std::string con_handler::get_table_error(size_t table) {
  if (VERBOSE) {
    cout << "Table error occured. Table numbered " << table << " is incorrect."
         << endl;
  }
  std::string result = "error table=";
  result += std::to_string(table);
  return result;
}

std::string con_handler::get_key_error(size_t key) {
  if (VERBOSE) {
    cout << "Key error occured. Key" << key << " is incorrect." << endl;
  }
  std::string result = "error key=";
  result += std::to_string(key);
  return result;
}

std::string con_handler::get_okey(size_t key, std::string value, size_t table) {
  if (VERBOSE) {
    cout << "Key and table are correct. Table number: " << table
         << " key: " << key << " value: " << value << endl;
  }
  std::string result = "ok key=";
  result += std::to_string(key);
  result += " value=";
  result += value;
  result += " table=";
  result += std::to_string(table);
  return result;
}