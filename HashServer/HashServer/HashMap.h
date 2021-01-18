#pragma once
#include <ctime>
#include <iostream>
#include <list>
#include <optional>
#include <string>
#include <utility>
#include <vector>


/**
 * \class HashMap
 *
 *
 * \brief Implements hash map.
 *
 * HashMap class has hash function that maps key values to bins (positions in vector).
 * Each element of the vector if a linked list, and elements are pushed back it the list.
 *
 * \author $Author: Liliya Makhmutova $
 *
 * \version $Revision: 1.0 $
 *
 * \date $Date: 2021/01/16 00:00:00 $
 */
class HashMap {
  /**
   * \struct Record
   *
   * \brief Each record constists of a key, value and expiration time.
   *
   * \author $Author: Liliya Makhmutova $
   *
   * \version $Revision: 1.0 $
   *
   * \date $Date: 2021/01/16 00:00:00 $
   */
  struct Record {
    int key;
    std::string value;
    time_t expires;
  };

 public:
  /**
   * A constructor.
   * Resizes the vector to some constant basic size BASIC_SIZE.
   */
  HashMap() { a.resize(BASIC_SIZE); }

  /** \brief Puts value by key with ttl to HashMap.
   * \param key to identify a record.
   * \param value to store value
   * \param ttl to store expiration time
   *
   * This method checks whether such key exists. 
   * If yes, it looks for it in a[h(key)] list and modifies its value.
   * Otherwise adds to a[h(key)] list new record.
   *
   */
  void put(int key, std::string value, size_t ttl);

  /** \brief Gets value by key from HashMap.
   * \param key to identify a record.
   *
   * This method checks whether such key exists.
   * If yes, it checks that it is not expired and returns its value.
   * Otherwise it returns nullopt.
   *
   * \return optional<string> object that is not nullopt 
   * when record exists and valid.
   */
  std::optional<std::string> get(int key);

  /** \brief Removes value by key from HashMap.
   * \param key to identify a record.
   *
   * This method looks for it in a[h(key)] list erases it if found.
   * 
   */
  void remove(int key);

  /** \brief Method that gets all contents of a table.
   *
   * This method iterates over all vector elements and inside each list.   * 
   *
   * \return Outputs string in the format: "key1:value1\nkey2:value2..."
   *
   */
  std::string get_table();

  /// Clears hash map
  void free_hash_map() { a.clear(); }

 private:
  static const size_t BASIC_SIZE = 50'000;
  std::vector<std::list<Record>> a;

  /** \brief Hash function implementation.
   * \param key to identify a record.
   * 
   * Large prime number was chosen to implement the hash function.
   *
   * \return Hash function mapping
   *
   */
  size_t h(int key) {
    size_t hash = (key * 2654435761) % BASIC_SIZE;
    return hash;
  }

  /** \brief This method checks whether time is expired.
   * \param time to check
   *
   * Compares expiration time with current time.
   *
   * \return Boolean value that identifies expiration.
   *
   */
  bool expired(time_t expires) { return expires < time(NULL); }
};
