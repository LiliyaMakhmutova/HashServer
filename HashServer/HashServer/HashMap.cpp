#include "HashMap.h"

void HashMap::put(int key, std::string value, size_t ttl) {
  time_t expires = time(NULL) + ttl;

  if (get(key) == std::nullopt) {
    a[h(key)].push_back({key, value, expires}); // add new 
  } else {
    for (auto& iter : a[h(key)]) {
      if (iter.key == key) { // change old value into the new one
        iter.value = value;
        iter.expires = expires;
        break;
      }
    }
  }
}

std::optional<std::string> HashMap::get(int key) {
  for (const auto& iter : a[h(key)]) {
    if (iter.key == key && !expired(iter.expires)) {
      return iter.value;
    }
  }
  return std::nullopt;
}

void HashMap::remove(int key) {
  for (auto it = a[h(key)].begin(); it != a[h(key)].end(); it++) {
    if ((*it).key == key) {
      a[h(key)].erase(it);
      break;
    }
  }
}

std::string HashMap::get_table() {
  std::string result;
  for (const auto& iter_v : a) {
    for (const auto& iter_l : iter_v) {
      if (!expired(iter_l.expires)) {
        result += std::to_string(iter_l.key) + ":" + iter_l.value + "\n";
      }
    }
  }
  return result.substr(0, result.size() - 1);  // remove \n last character
}