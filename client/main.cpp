#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../index_creator/t_output.h"
#include "Constants.h"
#include "Client.h"

using namespace boost::asio;

int main() {
	cons::enable_console_colors();
	cons::print("\t--- CLIENT STARTED ---", GREEN);

	std::vector<std::string> msg {"first message", "mice", "the", ""}; // messages to send

	Client client0(CON_IP, CON_PORT, { msg[0] });
	//Client client1(CON_IP, CON_PORT, { "message" });
	/*Client client2(CON_IP, CON_PORT, { msg[1], "hello" });
	Client client3(CON_IP, CON_PORT, { msg[3] });*/
	
	client0.join();
	//client1.join();
	/*client2.join();
	client3.join();*/

	cons::print("\t--- CLIENT STOPPED ---", RED);
	std::getchar(); // pause console
}