#include <boost/asio.hpp>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;
using std::string;
using std::vector;

std::mutex mutex_;

void FailWithMsg(const std::string& msg, int line) {
  std::cerr << "Test failed!\n";
  std::cerr << "[Line " << line << "] " << msg << std::endl;
  std::exit(EXIT_FAILURE);
}

#define ASSERT_TRUE(cond)                              \
  if (!(cond)) {                                       \
    FailWithMsg("Assertion failed: " #cond, __LINE__); \
  };

void test_user_request_response(const vector<string>& requests,
                                const vector<string>& responses) {
  std::lock_guard<std::mutex> lock_guard(mutex_);
  boost::asio::io_context io_context;

  
  for (int i = 0; i < requests.size(); i++) {    
    // socket creation
    tcp::socket socket(io_context);
    // connection
    socket.connect(tcp::endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"), 1234));
    // request from client  
    const string msg = requests[i];  
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);
    if (!error) {
      cout << "Client sent message! " << msg << endl;
    } else {
      cout << "send failed: " << error.message() << endl;
    }

    // response from server
    boost::asio::streambuf receive_buffer;
    boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(),
                      error);
    if (error && error != boost::asio::error::eof) {
      cout << "receive failed: " << error.message() << error.value() << endl;

    } else {
      const char* data =
          boost::asio::buffer_cast<const char*>(receive_buffer.data());
      cout << data << endl;
      ASSERT_TRUE(data == responses[i]);
    }    
  }
}

int main(int argc, char** argv) {
  setlocale(LC_ALL, "Russian");

  vector<vector<string>> all_user_requests;
  vector<vector<string>> all_user_responses;

  vector<string> requests1 = {"user1 addtable",
                              "user1 setval key=1 val=aaa table=0 ttl=10000",
                              "user1 getval key=1 table=0",
                              "user1 gettable 0",
                              "user1 remtable 0",
                              "user1 gettable 0"};
  vector<string> responses1 = {"0",     "", "ok key=1 value=aaa table=0",
                               "1:aaa", "", "error table=0"};

  vector<string> requests2 = {"user2 gettable 0", "user2 remtable 0"};
  vector<string> responses2 = {"error table=0", "error table=0"};

  all_user_requests = {requests1, requests2};
  all_user_responses = {responses1, responses2};

  std::thread thread1, thread2;
  std::vector<std::thread> threads;

  for (int i = 0; i < 2; ++i) {
    threads.push_back(std::thread(&test_user_request_response,
                                  all_user_requests[i], all_user_responses[i]));
  }
  for (int i = 0; i < threads.size(); i++) {
    threads.at(i).join();
  }

  return 0;
}