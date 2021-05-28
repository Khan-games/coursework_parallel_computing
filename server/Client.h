#pragma once

#include <boost/asio.hpp>

#include "Constants.h"
#include "t_output.h"

class Client
{
public:
	Client(boost::asio::ip::tcp::socket *sock, unsigned id);
	~Client();

	// task related
	void make_task(); // parallel method that recieves and make task from socket
	size_t read_complete(char* buff, const boost::system::error_code& err, size_t bytes);

	// get
	unsigned get_id();
	std::string get_ip();

private:
	// network vars
	boost::asio::ip::tcp::socket* sock;
	char buff[BUFF_SIZE];
	bool conn_terminated = false; // if true, then disconnect the client

	// multithread vars
	std::thread th_task;

	// other
	unsigned id;
	std::string msg;
};

