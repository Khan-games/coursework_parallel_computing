#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>

#include "Client.h"
#include "Constants.h"
#include "t_output.h"

class Server
{
public:
	Server();
	~Server();
	void listen(); // listen new connections 

private:
	// clients handler vars
	std::vector<Client*> clients;
	unsigned next_client_id = 0;

	// network vars
	boost::asio::io_service service; // main obj for boost::asio
	boost::asio::ip::tcp::acceptor acceptor; // receive connections

	// multithreads vars
	std::mutex clients_m; // responsible for changing clients handler vars
};

