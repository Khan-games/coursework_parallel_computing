#include <iostream>
#include <chrono>
#include <mutex>
#include <sstream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "t_output.h"

// network
#define EP_PORT 10105
#define EP_IP "127.0.0.1"

// constants
#define BUFF_SIZE 1024
#define BYTES_PER_READ 128 // one read operation from buffer

// for serialization
static auto const flags = boost::archive::no_header | boost::archive::no_tracking;


using namespace boost::asio;


// multithread vars
std::thread t;

// network vars
io_service service; // main obj for boost::asio
ip::tcp::endpoint ep(ip::address::from_string(EP_IP), EP_PORT); // connecting to localhost, port 10105


std::string thread_id_to_str() { // convert current thread's id to string
	std::stringstream ss;
	std::string s;
	ss << std::this_thread::get_id();
	ss >> s;
	return s;
}

size_t read_complete(char* buff, const boost::system::error_code& err,
	size_t bytes) {	// check if reading is done (return 0 if done)
	if (err) return 0;
	bool found = std::find(buff, buff + bytes, '\n') < (buff + bytes);
	return (!found) * BYTES_PER_READ;
}

//void send_msg(std::string msg) { // send msg to server
//	cons::print("Trying to send msg \"" + msg +"\"");
//	
//	msg += '\n'; // add indicator of msg end
//
//	boost::system::error_code ec;
//	ip::tcp::socket sock(service); // main socket for thread
//	sock.connect(ep, ec);
//	// check connection error
//	if (ec) {
//		cons::print( "[ERROR] Failed to connect to server!  thread_id = " + thread_id_to_str(), RED);
//		return;
//	}
//
//	sock.write_some(buffer(msg));
//
//	char buff[BUFF_SIZE];
//	int bytes = read(sock, buffer(buff), std::bind(read_complete, buff, std::placeholders::_1, 
//		std::placeholders::_2));
//
//	// check response
//	std::string response(buff, bytes);
//	if (msg == response) { // OK
//		cons::print("[RESP] Send msg \"" + msg.substr(0, msg.length()-1) + "\", received msg \"" 
//			+ response.substr(0, response.length()-1) +"\";  thread_id = " + thread_id_to_str(), GREEN);
//	}
//	else { // FAIL
//		cons::print("[RESP] Send msg \"" + msg.substr(0, msg.length() - 1) + "\", received msg \""
//			+ response.substr(0, response.length() - 1) + "\";  thread_id = " + thread_id_to_str(), GREEN);
//	}
//
//	
//}

void send_msg_serialized(std::string msg) { // send msg to server
	// connection
	cons::print("[NEW CONN] Connecting to server " + std::string(EP_IP) + ":" + std::to_string(EP_PORT));
	boost::system::error_code ec;
	ip::tcp::socket sock(service); // main socket for thread
	sock.connect(ep, ec);
	// check connection error
	if (ec) {
		cons::print("[ERROR] Failed to connect to server!  thread_id = " + thread_id_to_str(), RED);
		return;
	}
	cons::print("[NEW CONN] Connection established with server " + std::string(EP_IP) + ":" 
		+ std::to_string(EP_PORT) + ";  thread_id = " + thread_id_to_str(), GREEN);

	// archive data
	streambuf buff;
	boost::archive::binary_oarchive archive(buff, flags);
	archive << msg;
	cons::print("[ARCH] Data archived.  thread_id = " + thread_id_to_str());

	// send size
	size_t buff_size = buff.size();
	sock.send( buffer(&buff_size, sizeof(size_t)) );
	// send data
	int bytes = sock.send(buff.data(), buff_size);
	cons::print("[MSG] Data send.  thread_id = " + thread_id_to_str(), GREEN);
}

int main() {
	cons::enable_console_colors();
	cons::print("\t--- CLIENT STARTED ---", GREEN);

	boost::thread_group tg; // thread pool
	std::vector<std::string> messages {"first message", "second message", "third message", ""}; // messages to send

	for (auto msg : messages) {
		tg.create_thread(std::bind(send_msg_serialized, msg));
	}
	
	tg.join_all();

	cons::print("\t--- CLIENT STOPPED ---", RED);
	std::getchar(); // pause console
}