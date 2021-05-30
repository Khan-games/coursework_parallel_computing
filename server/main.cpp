#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "Server.h"
#include "../index_creator/t_output.h"
#include "Constants.h"

using namespace boost::asio;

// network vars
io_service service; // main obj for boost::asio

int main() {
	setlocale(0, ""); // for correct boost error typing
	cons::enable_console_colors();
	cons::print("\t--- SERVER STARTED ---", GREEN);
	
	Server server;
	server.listen();
	
	cons::print("\t--- SERVER STOPPED ---", RED);
	std::getchar(); // pause console
}