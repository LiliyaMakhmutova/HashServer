#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <vector>

#include "HashMap.h"
#include "HashServerConfig.h"

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;
struct Table;

extern std::vector<Table> tables;
extern size_t size;
extern std::mutex mutex_;
extern size_t ntables;
extern bool VERBOSE;


/**
 * \struct Table
 *
 *
 * \brief Table struct that stores hash table, creator and validity information
 *
 * When table is deleted, it becomes invalid. Each table has an owner.
 *
 *
 * \author $Author: Liliya Makhmutova $
 *
 * \version $Revision: 1.0 $
 *
 * \date $Date: 2021/01/16 00:00:00 $
 */
struct Table {
  std::string username;
  HashMap hash_map;
  bool valid;
};

/**
 * \class con_handler
 *
 *
 * \brief Handles each user connection, reads socket request and writes response
 * to the socket
 *
 * HashServer accepts a user connection, creates con_handler object.
 * This object asynchronically reads a socket, then if successful,
 * parses request and asynchronically writes response to the socket.
 * If unsuccessful, prints error to the stderr.
 * Prints dubug messages is VERBOSE mode is on.
 *
 *
 * \author $Author: Liliya Makhmutova $
 *
 * \version $Revision: 1.0 $
 *
 * \date $Date: 2021/01/16 00:00:00 $
 */
class con_handler : public boost::enable_shared_from_this<con_handler> {
 public:
  using ptr_to_connection = boost::shared_ptr<con_handler>;

  /**
   * A constructor.
   * Creates socket_ from io_context
   * \param io_context an io_context& argument.
   */
  explicit con_handler(boost::asio::io_context& io_context)
      : socket_(io_context) {}

  /**
   * A static member function that creates pointer to the connection
   * \param io_context an io_context& argument.
   */
  static ptr_to_connection create(boost::asio::io_context& io_context) {
    return ptr_to_connection(new con_handler(io_context));
  }

  /// socket getter
  tcp::socket& socket() { return socket_; }

  /// starts anync_read of the socket
  void start();

  /** \brief Method that invokes after user's response has been read.
   * \param err contains all information on error.
   * \param bytes_transferred contains number of bytes read
   *
   * This method checks whether no error occured.
   * If yes, it sends response asychronically.
   * If no, it prints error to stderr and closes the socket.
   *
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  void handle_read(const boost::system::error_code& err,
                   size_t bytes_transferred);

  /** \brief Method that invokes after user's response has been written.
   * \param err contains all information on error.
   * \param bytes_transferred contains number of bytes read
   *
   * This method checks whether no error occured.
   * If no, it prints error to stderr and closes the socket.
   *
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  void handle_write(const boost::system::error_code& err,
                    size_t bytes_transferred);

  /** \brief Method that adds table with username to tables.
   * \param username string contains username
   *
   * This method adds new table with empty hashmap, username.
   * Each table has a unique number (just like id field in database)
   * that equals to its position in vector of tables.
   * Increases size by 1.
   *
   * \warning this finction uses mutex lock_guard
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  std::string add_table(std::string username);

  /** \brief Method that gets all contents of a table with table_num number.
   * \param table_num table unique number
   *
   * This method gets all contents of a table with table_num number.
   *
   * \return Outputs string in the format: "key1:value1\nkey2:value2..."
   *
   * \warning this finction uses mutex lock_guard
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  std::string get_table(size_t table_num);

  /** \brief Method that sets value in table by key with ttl.
   * \param table_num table unique number
   * \param key in HashMap
   * \param val value in HashMap
   * \param ttl time in seconds that this value exists in the table
   *
   *
   * \warning this finction uses mutex lock_guard
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  void set_val(size_t table_num, int key, std::string val, time_t ttl);

  /** \brief Method that gets value in table by key.
   * \param table_num table unique number
   * \param key in HashMap
   *
   * \return optional<string> object that is not nullopt if value in the table
   * exists and alive.
   *
   *
   * \warning this finction uses mutex lock_guard
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  std::optional<std::string> get_val(size_t table_num, int key);

  /** \brief Method that removes table by table_num.
   * \param table_num table unique number
   *
   * \return empty string to send to the user.
   *
   * Make table invalid, clears HashMap and descreases size by 1.
   *
   * \warning this finction uses mutex lock_guard
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  std::string remove_table(size_t table_num);

  /** \brief Method that parses user request.
   * \param table_num table unique number
   *
   * \return string to send to the user.
   *
   * Checks validity of response.
   * Response could be: addtable, remtable, gettable, setval, getval.
   * If the response cannot be parsed, the function prints erroe to stderr.
   *
   * \warning this finction uses mutex lock_guard (it calls other functions that
   * cause lock) \note Is VERBOSE flag is set it prints debug messages to
   * stderr.
   */
  std::string parse_command_str(std::string str);

  /** \brief Method that checks whether table is valid.
   * \param table_num table unique number
   *
   * \return boolean value that indicates validity of a table
   *
   * Table is valid when its number exists and it has not been removed.
   *
   * \warning this finction uses mutex lock_guard
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  bool is_valid_table(size_t table_num);

  /** \brief Method that returns table error string.
   * \param table_num table unique number
   *
   * \return error string
   *
   * Returns "error table=table_num" string.
   *
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  std::string get_table_error(size_t table_num);

  /** \brief Method that returns key error string.
   * \param key value of key
   *
   * \return error string
   *
   * Returns "error key=table_num" string.
   *
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  std::string get_key_error(size_t key);

  /** \brief Method that returns ok string.
   * \param key value of key
   *
   * \return ok string
   *
   * Returns "ok key=key value=value table=table" string.
   *
   * \note Is VERBOSE flag is set it prints debug messages to stderr.
   */
  std::string get_okey(size_t key, std::string value, size_t table);

 private:
  static const size_t BUFFER_SIZE = 128;  /// fixed size buffer
  tcp::socket socket_;
  char in_message[BUFFER_SIZE];
  std::string out_message;
};

/**
 * \class HashServer
 *
 *
 * \brief Server that accepts and hadles user requests.
 *
 * HashServer accepts asynchronically a user connection, creates con_handler
 * object. After handling in in con_handler class, it invokes handle_accept.
 * This function that checks for errors and reconnects if they are found,
 * otherwise continues accepting.
 *
 * \author $Author: Liliya Makhmutova $
 *
 * \version $Revision: 1.0 $
 *
 * \date $Date: 2021/01/16 00:00:00 $
 */
class HashServer {
 public:
  /**
   * A constructor.
   * Creates socket_ from io_context and read server configuration from config.
   * 
   * \param io_context an io_context& argument
   * \param config contains all configurations of a server
   * 
   */
  HashServer(boost::asio::io_context& io_context, HashServerConfig config)
      : acceptor_(
            io_context,
            tcp::endpoint(boost::asio::ip::address::from_string(config.ip),
                          config.port)),
        io_context_(io_context) {
    start_accept();
    ntables = config.ntables;
    VERBOSE = config.verbose;
  }

  /** \brief Method that invokes after connection accepted.
  * 
   * \param connection pointer to a connection
   * \param err stores information on error 
   * 
   * If no error occured (or no EOF in socket) it starts asynchroniously read the socket.
   * Otherwise it starts accept new connection.
   *   
   */
  void handle_accept(con_handler::ptr_to_connection connection,
                     const boost::system::error_code& err) {
    if (!err) {
      connection->start();
    }
    start_accept();
  }

 private:
  tcp::acceptor acceptor_;
  io_context& io_context_;

  /** \brief Method that implements acception of connection.
   *
   * This method creates connection handler pointer (to avoid memory leakage).
   * After accept it will invoke handle_accept method to accept new connections
   * in a loop.
   *
   *
   */
  void start_accept() {
    // socket
    con_handler::ptr_to_connection connection =
        con_handler::create(io_context_);

    // asynchronically accept user response
    acceptor_.async_accept(
        connection->socket(),
        boost::bind(&HashServer::handle_accept, this, connection,
                    boost::asio::placeholders::error));
  }
};