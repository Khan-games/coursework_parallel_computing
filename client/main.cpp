#include <iostream>
#include <chrono>
#include <mutex>
#include <sstream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "t_output.h"

using namespace boost::asio;


// multithread vars
std::thread t;

// network vars
io_service service; // main obj for boost::asio
ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 10105); // localhost 

std::string thread_id_to_str() { // convert current thread's id to string
	std::stringstream ss;
	std::string s;
	ss << std::this_thread::get_id();
	ss >> s;
	return s;
}

void send_msg(std::string msg) {
	cons::print("Trying to send msg \"" + msg +"\"");
	
	boost::system::error_code ec;
	ip::tcp::socket sock(service);
	sock.connect(ep, ec);

	// check error
	if (ec) {
		cons::print( "[ERROR] Failed to connect to server!\tthread_id = " + thread_id_to_str(), RED);
	}


}

int main() {
	boost::thread_group tg; // thread pool
	std::vector<std::string> messages {"first message", "second message", "third message"}; // messages to send

	for (auto msg : messages) {
		tg.create_thread(std::bind(send_msg, msg));
	}
	
	tg.join_all();
}