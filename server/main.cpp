#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "t_output.h"

// network
#define CON_PORT 10105

// constants
#define BUFF_SIZE 1024 // total length of read msg
#define BYTES_PER_READ 128 // one read operation from buffer

using namespace boost::asio;

// multithread vars
//

// network vars
io_service service; // main obj for boost::asio


size_t read_complete(char* buff, const boost::system::error_code& err,
	size_t bytes) {	// check if reading is done (return 0 if done)

	if (err) return 0;
	bool found = std::find(buff, buff + bytes, '\n') < (buff + bytes);
	return (!found) * BYTES_PER_READ;
}

void handle_connections() {
	ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), CON_PORT)); // receive connections
	char buff[BUFF_SIZE]; 
	while (1) {
		// get connection
		ip::tcp::socket sock(service);
		acceptor.accept(sock); 
		cons::print("[NEW CONN] Connection established with IP " 
			+ sock.remote_endpoint().address().to_string(), GREEN);

		int bytes = read(sock, buffer(buff), std::bind(read_complete, buff, std::placeholders::_1,
			std::placeholders::_2)); // read msg
		
		// echo msg
		std::string msg(buff, bytes);
		if (msg.length()) {
			cons::print("[MSG] Received msg \"" + msg.substr(0, msg.length() - 1) + "\"", YELLOW);
		}
		else {
			cons::print("[MSG] Received msg is empty", RED);
		}
		sock.write_some(buffer(msg)); // echo to client
		
		sock.close();
	}

}

int main() {
	cons::enable_console_colors();
	cons::print("\t--- SERVER STARTED ---", GREEN);
	handle_connections();
	cons::print("\t--- SERVER STOPPED ---", RED);
	std::getchar(); // pause console
}