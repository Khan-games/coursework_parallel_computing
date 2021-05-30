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

	Client client0(CON_IP, CON_PORT, { "first message", "second_MESSAGE" });
	Client client1(CON_IP, CON_PORT, { "message", "1 2 3", "message thE, is " });
	Client client2(CON_IP, CON_PORT, { "first message", "hello", "other" });
	Client client3(CON_IP, CON_PORT, { "first message", "first message" });
	Client client4(CON_IP, CON_PORT, { "", "output", "" });
	
	client0.join();
	client1.join();
	client2.join();
	client3.join();
	client4.join();

	cons::print("\t--- CLIENT STOPPED ---", RED);
	std::getchar(); // pause console
}