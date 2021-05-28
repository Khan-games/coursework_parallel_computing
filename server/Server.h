#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>

#include "Client.h"
#include "Constants.h"
#include "t_output.h"

class Client;

class Server
{
public:
	Server();
	~Server();

	void listen(); // listen new connections 
	void disconnect_clients();

private:
	// clients handler vars
	std::list<Client*> clients;
	unsigned next_client_id = 0;

	// network vars
	boost::asio::io_service service; // main obj for boost::asio
	boost::asio::ip::tcp::acceptor acceptor; // receive connections

	// multithreads vars
	std::mutex clients_m; // responsible for changing clients list
	std::thread disconnect_th;
};

