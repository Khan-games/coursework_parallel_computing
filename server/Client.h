#pragma once

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "Constants.h"
#include "t_output.h"
#include "Server.h"

class Server;

class Client
{
public:
	Client(Server& server, boost::asio::ip::tcp::socket *sock, unsigned id);
	~Client();

	// task related
	void make_task(); // parallel method that recieves and make task from socket
	size_t read_complete(char* buff, const boost::system::error_code& err, size_t bytes);

	// data receive
	template<typename T>
	void read_client_data(T& data); // get data block from client

	// get
	unsigned get_id();
	std::string get_ip();
	bool is_disconnected();

private:
	// network vars
	boost::asio::ip::tcp::socket* sock;
	char buff[BUFF_SIZE];
	bool disconnected = false; // if true, then disconnect the client

	// multithread vars
	std::thread th_task;

	// other
	unsigned id;
	Server &host_server;
	
	// methods
	template<typename T>
	void read_data_once(T& data, size_t size); // get data block from client
};

