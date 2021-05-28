#include <iostream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "t_output.h"
#include "Constants.h"
#include "Client.h"

using namespace boost::asio;

int main() {
	cons::enable_console_colors();
	cons::print("\t--- CLIENT STARTED ---", GREEN);

	std::vector<std::string> messages {"first message", "second message", "third message", ""}; // messages to send

	Client client0(CON_IP, CON_PORT, { "first message" });
	Client client1(CON_IP, CON_PORT, { "second message" });
	Client client2(CON_IP, CON_PORT, { "third message" });
	Client client3(CON_IP, CON_PORT, { "" });
	Client client4(CON_IP, CON_PORT, { "__first message", "__second message", "__third message", "__" });

	cons::print("HELLO", CYAN);
	
	client0.join();
	client1.join();
	client2.join();
	client3.join();
	client4.join();

	cons::print("\t--- CLIENT STOPPED ---", RED);
	std::getchar(); // pause console
}