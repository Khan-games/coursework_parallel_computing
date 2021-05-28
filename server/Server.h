#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>

#include "Client.h"
#include "Constants.h"

class Server
{
public:
	Server();
	void listen(); // listen new connections 

private:
	// clients handler vars
	std::vector<Client> clients;
	std::vector<std::thread> th_clients;

	// network vars
	boost::asio::io_service service; // main obj for boost::asio
	boost::asio::ip::tcp::acceptor acceptor; // receive connections
};

